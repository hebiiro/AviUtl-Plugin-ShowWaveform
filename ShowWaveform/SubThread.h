#pragma once

#include "Share.h"
#include "Sender.h"

//--------------------------------------------------------------------

struct SubThread
{
	static const UINT WM_SEND = WM_APP + 1;

	// �T�u�X���b�h���̏����B
	SubThread(HWND hwnd);
	~SubThread();
	void onSend(Sender* sender);
	DWORD proc();
};

struct SubThreadManager
{
	Mutex m_mutex;
	SimpleFileMapping m_fileMapping;

	DWORD m_tid = 0;
	Handle m_handle;

	// ���C���X���b�h���̏����B
	BOOL init(AviUtl::FilterPlugin* fp);
	BOOL exit(AviUtl::FilterPlugin* fp);
	BOOL send(LPCSTR fileName);

	// �T�u�X���b�h���̏����B
	static DWORD CALLBACK threadProc(LPVOID param);
};

//--------------------------------------------------------------------
