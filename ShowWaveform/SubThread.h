#pragma once

#include "ShowWaveform.h"

//--------------------------------------------------------------------

struct CacheRequest
{
	std::string fileName;

	// メインスレッド側の処理。
	CacheRequest(LPCSTR fileName);
};

struct SubThread
{
	static const UINT WM_POST_CACHE_REQUEST = WM_APP + 1;
	static const UINT WM_SEND_CACHE_REQUEST = WM_APP + 2;
	static const UINT WM_SEND_PROJECT_CHANGED = WM_APP + 3;
	static const UINT WM_SEND_ITEM_CHANGED = WM_APP + 4;
	static const UINT WM_POST_CLEAR_REQUEST = WM_APP + 5;
	static const UINT WM_POST_REDRAW_REQUEST = WM_APP + 6;

	std::map<std::string, const CacheRequest*> cacheRequestMap;

	// サブスレッド側の処理。
	SubThread(HWND hwnd);
	~SubThread();
	void onPostCacheRequest(const CacheRequest* cacheRequest);
	void onSendCacheRequest(const CacheRequest* cacheRequest);
	void onSendProjectChanged(const ProjectParams* params);
	void onSendItemChanged(const AudioParams* params);
	void onPostClearRequest();
	void onPostRedrawRequest();
	DWORD proc();
};

struct SubThreadManager
{
	Mutex m_mutex;
	SimpleFileMapping m_fileMapping;
	SimpleFileMapping m_fileMappingProjectParams;
	SimpleFileMapping m_fileMappingAudioParams;

	DWORD m_tid = 0;
	Handle m_handle;

	// メインスレッド側の処理。
	BOOL init(AviUtl::FilterPlugin* fp);
	BOOL exit(AviUtl::FilterPlugin* fp);
	BOOL requestCache(LPCSTR fileName);
	BOOL notifyProjectChanged(const ProjectParamsPtr& params);
	BOOL notifyItemChanged(const AudioParamsPtr& params);
	BOOL requestClear();
	BOOL requestRedraw();

	// サブスレッド側の処理。
	static DWORD CALLBACK threadProc(LPVOID param);
};

//--------------------------------------------------------------------
