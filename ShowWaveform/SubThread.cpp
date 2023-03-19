#include "pch.h"
#include "SubThread.h"
#include "App.h"

//--------------------------------------------------------------------

BOOL SubThreadManager::init(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubThreadManager::init()\n"));

	m_mutex.reset(new Mutex(0, FALSE, FormatText(_T("ShowWaveform.Mutex.%08X"), fp->hwnd)));
	m_fileMapping.reset(new SimpleFileMapping(sizeof(Bottle), FormatText(_T("ShowWaveform.FileMapping.%08X"), fp->hwnd)));

	m_hwnd = fp->hwnd;
	m_handle = ::CreateThread(0, 0, threadProc, this, 0, &m_tid);
	return !!m_handle;
}

BOOL SubThreadManager::exit(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubThreadManager::exit()\n"));

	return ::PostThreadMessage(m_tid, WM_QUIT, 0, 0);
}

BOOL SubThreadManager::send(LPCSTR fileName)
{
	MY_TRACE(_T("SubThreadManager::send()\n"));

	return ::PostThreadMessage(m_tid, WM_SEND, (WPARAM)new Sender(fileName), 0);
}

//--------------------------------------------------------------------

void SubThreadManager::onSend(Sender* sender)
{
	if (m_mutex && m_fileMapping)
	{
//		Synchronizer sync(*m_mutex);
		Bottle* bottle = (Bottle*)m_fileMapping->getBuffer();

		memcpy(&bottle->fileName, &sender->fileName, sizeof(bottle->fileName));
		bottle->sampleCount = 0;
	}

	::SendMessage(theApp.m_subProcess.m_mainWindow, WM_AVIUTL_FILTER_SEND, 0, 0);

	delete sender;
}

DWORD SubThreadManager::threadProc()
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

DWORD CALLBACK SubThreadManager::threadProc(LPVOID param)
{
	SubThreadManager* senderThread = (SubThreadManager*)param;
	return senderThread->threadProc();
}

//--------------------------------------------------------------------
