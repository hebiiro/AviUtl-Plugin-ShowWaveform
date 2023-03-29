#include "pch.h"
#include "ReaderProcess.h"
#include "Common/Tracer2.h"

//--------------------------------------------------------------------

BOOL ReaderProcess::init(HINSTANCE instance)
{
	MY_TRACE(_T("ReaderProcess::init()\n"));

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

BOOL ReaderProcess::term()
{
	MY_TRACE(_T("ReaderProcess::term()\n"));

	return termPlugin();
}

BOOL ReaderProcess::initPlugin()
{
	MY_TRACE(_T("ReaderProcess::initPlugin()\n"));

	TCHAR fileName[MAX_PATH] = {};
	::GetModuleFileName(0, fileName, MAX_PATH);
	::PathRemoveFileSpec(fileName);
	::PathAppend(fileName, _T("..\\lwinput.aui"));
	MY_TRACE_TSTR(fileName);

	m_plugin = std::make_shared<Input::Plugin>(fileName);

	return !!m_plugin->getInputPlugin();
}

BOOL ReaderProcess::termPlugin()
{
	MY_TRACE(_T("ReaderProcess::termPlugin()\n"));

	m_plugin = 0;

	return TRUE;
}

// 入力プラグインから音声サンプルを受け取る。
BOOL ReaderProcess::receive()
{
	MY_TRACE(_T("ReaderProcess::receive()\n"));

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
	int32_t length = mi->audio_format.nSamplesPerSec / SAMPLE_FPS;
	int32_t bufferSize = mi->audio_format.nBlockAlign * length;
	std::vector<BYTE> buffer(bufferSize);

	MY_TRACE_INT(mi->audio_format.wFormatTag);
	MY_TRACE_INT(mi->audio_format.nChannels);
	MY_TRACE_INT(mi->audio_format.nSamplesPerSec);
	MY_TRACE_INT(mi->audio_format.nAvgBytesPerSec);
	MY_TRACE_INT(mi->audio_format.nBlockAlign);
	MY_TRACE_INT(mi->audio_format.wBitsPerSample);
	MY_TRACE_INT(length);
	MY_TRACE_INT(bufferSize);

	// 入力プラグインを使用して音声サンプルを読み込む。

	MY_TRACE_INT(sizeof(int24_t));

	DWORD startTime = ::timeGetTime();

	shared->sampleCount = 0;

	for (int i = 0; i < MAX_SAMPLE_COUNT; i++)
	{
		int32_t read = ip->func_read_audio(handle, start, length, buffer.data());
		start += read;

		Sample sample;
		switch (mi->audio_format.wBitsPerSample)
		{
		case 8: sample.level = calc((const int8_t*)buffer.data(), (int)(buffer.size() / sizeof(int8_t))); break;
		case 16: sample.level = calc((const int16_t*)buffer.data(), (int)(buffer.size() / sizeof(int16_t))); break;
		case 24: sample.level = calc((const int24_t*)buffer.data(), (int)(buffer.size() / sizeof(int24_t))); break;
		case 32: sample.level = calc((const int32_t*)buffer.data(), (int)(buffer.size() / sizeof(int32_t))); break;
		default: sample.level = 0.0f; break;
		}
		shared->samples[i] = sample;
		shared->sampleCount++;

		MY_TRACE(_T("i = %d, read = %d, level = %f\n"), i, read, sample.level);

		if (read < length)
			break;
	}

	DWORD endTime = ::timeGetTime();

	MY_TRACE(_T("所要時間 = %f秒\n"), (endTime - startTime) / 1000.0);

	return TRUE;
}

// クライアントプロセスに音声サンプルを送る。(受け取るように促す)
BOOL ReaderProcess::send()
{
	MY_TRACE(_T("ReaderProcess::send()\n"));

	DWORD id = ::GetCurrentThreadId();
	return ::PostMessage(m_client, WM_AVIUTL_FILTER_RECEIVE, (WPARAM)id, 0);
}

//--------------------------------------------------------------------

inline float ReaderProcess::normalize(int8_t pcm)
{
	return pcm / 128.0f; // 8bit を -1.0 ～ 1.0 に正規化
}

inline float ReaderProcess::normalize(int16_t pcm)
{
	return pcm / 32768.0f; // 16bit を -1.0 ～ 1.0 に正規化
}

inline float ReaderProcess::normalize(int24_t pcm)
{
	return pcm / 8388608.0f; // 24bit を -1.0 ～ 1.0 に正規化
}

inline float ReaderProcess::normalize(int32_t pcm)
{
	return pcm / 2147483648.0f; // 32bit を -1.0 ～ 1.0 に正規化
}

inline float ReaderProcess::normalize(float pcm)
{
	return pcm;
}

template<typename T>
float ReaderProcess::calc(const T* samples, int count)
{
	if (!count) return 0.0f;

	float level = 0.0f;
	for (int i = 0; i < count; i++)
	{
		T sample = samples[i];
		float n = normalize(sample);
		level += n * n;
	}
	return sqrtf(level / (float)count);
}

//--------------------------------------------------------------------

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	struct Initializer
	{
		Initializer()
		{
			_tsetlocale(LC_ALL, _T(""));
			trace_init(0, 0, TRUE);
			::OleInitialize(0);
		}

		~Initializer()
		{
			trace_term();
			::OleUninitialize();
		}

	} initializer;

	MY_TRACE(_T("WinMain()\n"));

	ReaderProcess reader;

	reader.init(instance);
	reader.receive();
	reader.send();
	reader.term();

	MY_TRACE(_T("プロセスが正常終了しました\n"));

	return 0;
}

//--------------------------------------------------------------------
