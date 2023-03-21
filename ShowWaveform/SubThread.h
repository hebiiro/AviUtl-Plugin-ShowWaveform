#pragma once

#include "Share.h"
#include "Sender.h"

//--------------------------------------------------------------------

struct SubThread
{
	static const UINT WM_SEND = WM_APP + 1;

	// サブスレッド側の処理。
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

	// メインスレッド側の処理。
	BOOL init(AviUtl::FilterPlugin* fp);
	BOOL exit(AviUtl::FilterPlugin* fp);
	BOOL send(LPCSTR fileName);

	// サブスレッド側の処理。
	static DWORD CALLBACK threadProc(LPVOID param);
};

//--------------------------------------------------------------------
