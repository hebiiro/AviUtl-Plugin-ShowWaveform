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

	// メインスレッド側の処理。
	BOOL init(AviUtl::FilterPlugin* fp);
	BOOL exit(AviUtl::FilterPlugin* fp);
	BOOL send(LPCSTR fileName);

	// サブスレッド側の処理。
	void onSend(Sender* sender);
	DWORD threadProc();
	static DWORD CALLBACK threadProc(LPVOID param);
};

//--------------------------------------------------------------------
