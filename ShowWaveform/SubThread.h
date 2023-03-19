#pragma once

#include "Share.h"
#include "Sender.h"

//--------------------------------------------------------------------

struct SubThreadManager
{
	static const UINT WM_SEND = WM_APP + 1;

	HWND m_hwnd = 0;
	DWORD m_tid = 0;
	Handle m_handle;
	std::unique_ptr<Mutex> m_mutex;
	std::unique_ptr<SimpleFileMapping> m_fileMapping;

	// ���C���X���b�h���̏����B
	BOOL init(AviUtl::FilterPlugin* fp);
	BOOL exit(AviUtl::FilterPlugin* fp);
	BOOL send(LPCSTR fileName);

	// �T�u�X���b�h���̏����B
	void onSend(Sender* sender);
	DWORD threadProc();
	static DWORD CALLBACK threadProc(LPVOID param);
};

//--------------------------------------------------------------------
