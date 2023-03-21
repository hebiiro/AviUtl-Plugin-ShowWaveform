#include "pch.h"
#include "SubThread.h"
#include "App.h"

//--------------------------------------------------------------------

SubThread::SubThread(HWND hwnd)
{
	MY_TRACE(_T("SubThread::SubThread(0x%08X)\n"), hwnd);
}

SubThread::~SubThread()
{
	MY_TRACE(_T("SubThread::~SubThread()\n"));
}

void SubThread::onSend(Sender* sender)
{
	{
//		Synchronizer sync(theApp.m_subThreadManager.m_mutex);
		Bottle* bottle = (Bottle*)theApp.m_subThreadManager.m_fileMapping.getBuffer();

		if (bottle)
		{
			memcpy(&bottle->fileName, &sender->fileName, sizeof(bottle->fileName));
			bottle->sampleCount = 0;
		}
	}

	::SendMessage(theApp.m_subProcess.m_mainWindow, WM_AVIUTL_FILTER_SEND, 0, 0);

	delete sender;
}

DWORD SubThread::proc()
{
	MSG msg = {};
	while (::GetMessage(&msg, 0, 0, 0))
	{
		if (!msg.hwnd)
		{
			switch (msg.message)
			{
			case WM_SEND:
				{
					onSend((Sender*)msg.wParam);
					break;
				}
			case WM_QUIT:
				{
					return 0;
				}
			}
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return 0;
}

//--------------------------------------------------------------------

BOOL SubThreadManager::init(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubThreadManager::init()\n"));

	m_mutex.init(0, FALSE, FormatText(_T("ShowWaveform.Mutex.%08X"), fp->hwnd));
	m_fileMapping.init(sizeof(Bottle), FormatText(_T("ShowWaveform.FileMapping.%08X"), fp->hwnd));

	m_handle = ::CreateThread(0, 0, threadProc, fp->hwnd, 0, &m_tid);
	return !!m_handle;
}

BOOL SubThreadManager::exit(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubThreadManager::exit()\n"));

//	return ::PostThreadMessage(m_tid, WM_QUIT, 0, 0);
	return ::TerminateThread(m_handle, -1);
}

BOOL SubThreadManager::send(LPCSTR fileName)
{
	MY_TRACE(_T("SubThreadManager::send()\n"));

	return ::PostThreadMessage(m_tid, SubThread::WM_SEND, (WPARAM)new Sender(fileName), 0);
}

//--------------------------------------------------------------------

DWORD CALLBACK SubThreadManager::threadProc(LPVOID param)
{
	SubThread subThread((HWND)param);

	return subThread.proc();
}

//--------------------------------------------------------------------
