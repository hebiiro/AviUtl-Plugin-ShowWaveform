#pragma once

#include "Plugin.h"
#include "Calc/Hive.h"
#include "Calc/Worker.h"

inline struct App
{
	HINSTANCE m_instance;
	HWND m_client;
	Event m_event;
	SimpleFileMappingT<ReaderBottle> m_shared;
	Input::PluginPtr m_plugin;
	Input::MediaPtr m_media;

	BOOL init(HINSTANCE instance)
	{
		MY_TRACE(_T("App::init()\n"));

		m_instance = instance;

		// クライアントプロセスのウィンドウハンドルを取得する。
		m_client = (HWND)_tcstoul(::GetCommandLine(), 0, 0);
		MY_TRACE_HEX(m_client);
		if (!m_client) return FALSE;

		// ここはクライアントプロセスより先に処理される可能性がある。
		// その場合、イベントのオープンには失敗してもよい。
		// 共有メモリの確保は成功するのが前提なので open() は使用できない。

		DWORD tid = ::GetCurrentThreadId();
		m_event.open(EVENT_ALL_ACCESS, FALSE, getReaderEventName(tid));
		m_shared.init(getSharedReaderBottleName(tid));

		return initPlugin();
	}

	BOOL term()
	{
		MY_TRACE(_T("App::term()\n"));

		return termPlugin();
	}

	BOOL initPlugin()
	{
		MY_TRACE(_T("App::initPlugin()\n"));

		TCHAR fileName[MAX_PATH] = {};
		::GetModuleFileName(0, fileName, MAX_PATH);
		::PathRemoveFileSpec(fileName);
		::PathAppend(fileName, _T("..\\lwinput.aui"));
		MY_TRACE_TSTR(fileName);

		m_plugin = std::make_shared<Input::Plugin>(fileName);

		return !!m_plugin->getInputPlugin();
	}

	BOOL termPlugin()
	{
		MY_TRACE(_T("App::termPlugin()\n"));

		m_plugin = 0;

		return TRUE;
	}

	//
	// 入力プラグインから音声信号を受け取ります。
	// 同時並行して音量を算出します。
	//
	BOOL receive()
	{
		MY_TRACE(_T("App::receive()\n"));

		// イベントが発生するまで待つ。
		if (m_event)
			::WaitForSingleObject(m_event, INFINITE);

		// 共有メモリを取得する。
		ReaderBottle* shared = m_shared.getBuffer();
		if (!shared) return FALSE;

		// 入力プラグインのインターフェイスを取得する。
		AviUtl::InputPluginDLL* ip = m_plugin->getInputPlugin();
		MY_TRACE_HEX(ip);
		if (!ip) return FALSE;

		// メディアを開く。
		Input::MediaPtr media = std::make_shared<Input::Media>(m_plugin, shared->fileName);

		// メディアのハンドルを取得する。
		AviUtl::InputHandle handle = media->getInputHandle();
		MY_TRACE_HEX(handle);
		if (!handle) return FALSE;

		// メディア情報を取得する。
		MediaInfo* mi = media->getMediaInfo();

		// バッファのサイズを算出し、バッファを確保する。
		int32_t start = 0;
		int32_t length = mi->audio_format.nSamplesPerSec / Volume::Resolution;
		int32_t bufferSize = length * mi->audio_format.nBlockAlign;

		MY_TRACE_INT(mi->audio_format.wFormatTag);
		MY_TRACE_INT(mi->audio_format.nChannels);
		MY_TRACE_INT(mi->audio_format.nSamplesPerSec);
		MY_TRACE_INT(mi->audio_format.nAvgBytesPerSec);
		MY_TRACE_INT(mi->audio_format.nBlockAlign);
		MY_TRACE_INT(mi->audio_format.wBitsPerSample);
		MY_TRACE_INT(length);
		MY_TRACE_INT(bufferSize);

		// 入力プラグインを使用して音声信号を読み込む。

		DWORD startTime = ::timeGetTime();

		// 計算に必要なデータをハイブに格納しておく。
		Calc::hive.setBottle(shared);
		Calc::hive.setAudioFormat(mi->audio_format);

		// ワークの配列。
		std::vector<PTP_WORK> works(Volume::MaxCount);

		// 読み込んだフレーム数。
		int c = Volume::MaxCount;

		// 最大フレーム数までループする。
		// ただし、読み込む音声信号がなくなった場合はそこでループは終了する。
		for (int i = 0; i < Volume::MaxCount; i++)
		{
			// 音声信号を受け取るためのバッファを確保する。
			Calc::BufferPtr buffer = std::make_shared<Calc::Buffer>(bufferSize);

			// AviUtl の入力プラグインを使用して音声信号を取得する。
			int32_t read = ip->func_read_audio(handle, start, length, buffer->data());

			if (read == 0) // 音声信号を読み込めなかった場合は
			{
				c = i; // 読み込んだフレーム数をセットしてから

				break; // ループを終了する。
			}

			// 読み取った分だけ start を進める。
			start += read;

			// バッファのサイズを読み取った分だけにする。
			buffer->resize(read * mi->audio_format.nBlockAlign);

			// ワーカーを作成する。
			Calc::Worker* worker = Calc::Worker::create(i, buffer);

			// スレッドプールワークを作成する。
			works[i] = ::CreateThreadpoolWork(Calc::Worker::WorkCallback, worker, 0);

			// スレッドプールワークを開始する。
			::SubmitThreadpoolWork(works[i]);

			if (read < length) // 音声信号が想定より少なかった場合は
			{
				c = i + 1; // 読み込んだフレーム数をセットしてから

				break; // ループを終了する。
			}
		}

		// 読み込んだフレーム数。
		shared->volumeCount = c;

		// 読み込んだフレーム数の分だけ、スレッドプールワークの終了を待つ。
		for (int i = 0; i < c; i++)
		{
			::WaitForThreadpoolWorkCallbacks(works[i], FALSE);
			::CloseThreadpoolWork(works[i]);
		}

		DWORD endTime = ::timeGetTime();

		MY_TRACE(_T("所要時間 = %f秒\n"), (endTime - startTime) / 1000.0);

		return TRUE;
	}

	//
	// クライアントプロセスに算出した音量を送ります。(受け取るように促します)
	//
	BOOL send()
	{
		MY_TRACE(_T("App::send()\n"));

		DWORD id = ::GetCurrentThreadId(); // 共有メモリを識別するために必要です。

		return ::PostMessage(m_client, WM_AVIUTL_FILTER_RECEIVE, (WPARAM)id, 0);
	}
} app;
