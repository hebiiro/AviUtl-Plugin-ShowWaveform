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

		// �N���C�A���g�v���Z�X�̃E�B���h�E�n���h�����擾����B
		m_client = (HWND)_tcstoul(::GetCommandLine(), 0, 0);
		MY_TRACE_HEX(m_client);
		if (!m_client) return FALSE;

		// �����̓N���C�A���g�v���Z�X����ɏ��������\��������B
		// ���̏ꍇ�A�C�x���g�̃I�[�v���ɂ͎��s���Ă��悢�B
		// ���L�������̊m�ۂ͐�������̂��O��Ȃ̂� open() �͎g�p�ł��Ȃ��B

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
	// ���̓v���O�C�����特���M�����󂯎��܂��B
	// �������s���ĉ��ʂ��Z�o���܂��B
	//
	BOOL receive()
	{
		MY_TRACE(_T("App::receive()\n"));

		// �C�x���g����������܂ő҂B
		if (m_event)
			::WaitForSingleObject(m_event, INFINITE);

		// ���L���������擾����B
		ReaderBottle* shared = m_shared.getBuffer();
		if (!shared) return FALSE;

		// ���̓v���O�C���̃C���^�[�t�F�C�X���擾����B
		AviUtl::InputPluginDLL* ip = m_plugin->getInputPlugin();
		MY_TRACE_HEX(ip);
		if (!ip) return FALSE;

		// ���f�B�A���J���B
		Input::MediaPtr media = std::make_shared<Input::Media>(m_plugin, shared->fileName);

		// ���f�B�A�̃n���h�����擾����B
		AviUtl::InputHandle handle = media->getInputHandle();
		MY_TRACE_HEX(handle);
		if (!handle) return FALSE;

		// ���f�B�A�����擾����B
		MediaInfo* mi = media->getMediaInfo();

		// �o�b�t�@�̃T�C�Y���Z�o���A�o�b�t�@���m�ۂ���B
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

		// ���̓v���O�C�����g�p���ĉ����M����ǂݍ��ށB

		DWORD startTime = ::timeGetTime();

		// �v�Z�ɕK�v�ȃf�[�^���n�C�u�Ɋi�[���Ă����B
		Calc::hive.setBottle(shared);
		Calc::hive.setAudioFormat(mi->audio_format);

		// ���[�N�̔z��B
		std::vector<PTP_WORK> works(Volume::MaxCount);

		// �ǂݍ��񂾃t���[�����B
		int c = Volume::MaxCount;

		// �ő�t���[�����܂Ń��[�v����B
		// �������A�ǂݍ��މ����M�����Ȃ��Ȃ����ꍇ�͂����Ń��[�v�͏I������B
		for (int i = 0; i < Volume::MaxCount; i++)
		{
			// �����M�����󂯎�邽�߂̃o�b�t�@���m�ۂ���B
			Calc::BufferPtr buffer = std::make_shared<Calc::Buffer>(bufferSize);

			// AviUtl �̓��̓v���O�C�����g�p���ĉ����M�����擾����B
			int32_t read = ip->func_read_audio(handle, start, length, buffer->data());

			if (read == 0) // �����M����ǂݍ��߂Ȃ������ꍇ��
			{
				c = i; // �ǂݍ��񂾃t���[�������Z�b�g���Ă���

				break; // ���[�v���I������B
			}

			// �ǂݎ���������� start ��i�߂�B
			start += read;

			// �o�b�t�@�̃T�C�Y��ǂݎ�����������ɂ���B
			buffer->resize(read * mi->audio_format.nBlockAlign);

			// ���[�J�[���쐬����B
			Calc::Worker* worker = Calc::Worker::create(i, buffer);

			// �X���b�h�v�[�����[�N���쐬����B
			works[i] = ::CreateThreadpoolWork(Calc::Worker::WorkCallback, worker, 0);

			// �X���b�h�v�[�����[�N���J�n����B
			::SubmitThreadpoolWork(works[i]);

			if (read < length) // �����M�����z���菭�Ȃ������ꍇ��
			{
				c = i + 1; // �ǂݍ��񂾃t���[�������Z�b�g���Ă���

				break; // ���[�v���I������B
			}
		}

		// �ǂݍ��񂾃t���[�����B
		shared->volumeCount = c;

		// �ǂݍ��񂾃t���[�����̕������A�X���b�h�v�[�����[�N�̏I����҂B
		for (int i = 0; i < c; i++)
		{
			::WaitForThreadpoolWorkCallbacks(works[i], FALSE);
			::CloseThreadpoolWork(works[i]);
		}

		DWORD endTime = ::timeGetTime();

		MY_TRACE(_T("���v���� = %f�b\n"), (endTime - startTime) / 1000.0);

		return TRUE;
	}

	//
	// �N���C�A���g�v���Z�X�ɎZ�o�������ʂ𑗂�܂��B(�󂯎��悤�ɑ����܂�)
	//
	BOOL send()
	{
		MY_TRACE(_T("App::send()\n"));

		DWORD id = ::GetCurrentThreadId(); // ���L�����������ʂ��邽�߂ɕK�v�ł��B

		return ::PostMessage(m_client, WM_AVIUTL_FILTER_RECEIVE, (WPARAM)id, 0);
	}
} app;
