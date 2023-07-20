#include "pch.h"
#include "SubThread.h"
#include "App.h"

//--------------------------------------------------------------------

Shared shared;

//--------------------------------------------------------------------

CacheRequest::CacheRequest(LPCSTR fileName)
	: fileName(fileName)
{
}

//--------------------------------------------------------------------

SubThread::SubThread()
{
	MY_TRACE(_T("SubThread::SubThread()\n"));
}

SubThread::~SubThread()
{
	MY_TRACE(_T("SubThread::~SubThread()\n"));
}

void SubThread::onPostCacheRequest(const CacheRequest* cacheRequest)
{
	MY_TRACE(_T("SubThreadManager::onPostCacheRequest()\n"));

	auto it = cacheRequestMap.find(cacheRequest->fileName);
	if (it != cacheRequestMap.end())
	{
		// 同一のリクエストがマップ内に残っているので
		// このリクエストは破棄する。
		delete cacheRequest;
		MY_TRACE(_T("リクエストを破棄しました\n"));
		return;
	}

	cacheRequestMap[cacheRequest->fileName] = cacheRequest;

	::PostThreadMessage(::GetCurrentThreadId(), SubThread::WM_SEND_CACHE_REQUEST, (WPARAM)cacheRequest, 0);
}

void SubThread::onSendCacheRequest(const CacheRequest* cacheRequest)
{
	MY_TRACE(_T("SubThreadManager::onSendCacheRequest()\n"));

	// 送信用のボトルを取得する。
	SenderBottle* shared = ::shared.senderBottle.getBuffer();
	if (shared)
	{
		::StringCbCopyA(shared->fileName, sizeof(shared->fileName), cacheRequest->fileName.c_str());

		// サブプロセスにキャッシュの作成をリクエストする。
		::SendMessage(theApp.m_subProcess.m_windowContainer.m_inner, WM_AVIUTL_FILTER_SEND, SendID::requestCache, 0);
	}

	// このリクエストをマップから削除する。
	auto it = cacheRequestMap.find(cacheRequest->fileName);
	if (it != cacheRequestMap.end())
	{
		delete it->second;
		cacheRequestMap.erase(it);
	}
}

void SubThread::onSendProjectChanged(const ProjectParams* params)
{
	MY_TRACE(_T("SubThreadManager::onSendProjectChanged()\n"));

	ProjectParams* shared = ::shared.projectParams.getBuffer();
	if (shared)
	{
		*shared = *params;

		// サブプロセスにアイテムの変更を通知する。
		::SendMessage(theApp.m_subProcess.m_windowContainer.m_inner, WM_AVIUTL_FILTER_SEND, SendID::notifyProjectChanged, 0);
	}

	delete params;
}

void SubThread::onSendItemChanged(const AudioParams* params)
{
	MY_TRACE(_T("SubThreadManager::onSendItemChanged()\n"));

	AudioParams* shared = ::shared.audioParams.getBuffer();
	if (shared)
	{
		*shared = *params;

		// サブプロセスにアイテムの変更を通知する。
		::SendMessage(theApp.m_subProcess.m_windowContainer.m_inner, WM_AVIUTL_FILTER_SEND, SendID::notifyItemChanged, 0);
	}

	delete params;
}

void SubThread::onSendTotalsChanged(const TotalsParams* params)
{
	MY_TRACE(_T("SubThreadManager::onSendTotalsChanged()\n"));

	TotalsParams* shared = ::shared.senderTotalsParams.getBuffer();
	if (shared)
	{
		*shared = *params;

		// サブプロセスにサブプロセスパラメータの変更を通知する。
		::SendMessage(theApp.m_subProcess.m_windowContainer.m_inner, WM_AVIUTL_FILTER_SEND, SendID::notifyTotalsChanged, 0);
	}

	delete params;
}

void SubThread::onPostClearRequest()
{
	MY_TRACE(_T("SubThreadManager::onPostClearRequest()\n"));

	::SendMessage(theApp.m_subProcess.m_windowContainer.m_inner, WM_AVIUTL_FILTER_CLEAR, 0, 0);
}

void SubThread::onPostRedrawRequest()
{
	MY_TRACE(_T("SubThreadManager::onPostRedrawRequest()\n"));

	::SendMessage(theApp.m_subProcess.m_windowContainer.m_inner, WM_AVIUTL_FILTER_REDRAW, 0, 0);
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
			case WM_POST_CACHE_REQUEST:
				{
					onPostCacheRequest((const CacheRequest*)msg.wParam);
					break;
				}
			case WM_SEND_CACHE_REQUEST:
				{
					onSendCacheRequest((const CacheRequest*)msg.wParam);
					break;
				}
			case WM_SEND_PROJECT_CHANGED:
				{
					onSendProjectChanged((const ProjectParams*)msg.wParam);
					break;
				}
			case WM_SEND_ITEM_CHANGED:
				{
					onSendItemChanged((const AudioParams*)msg.wParam);
					break;
				}
			case WM_SEND_TOTALS_CHANGED:
				{
					onSendTotalsChanged((const TotalsParams*)msg.wParam);
					break;
				}
			case WM_POST_CLEAR_REQUEST:
				{
					onPostClearRequest();
					break;
				}
			case WM_POST_REDRAW_REQUEST:
				{
					onPostRedrawRequest();
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

	HWND hwnd = theApp.m_subProcess.m_windowContainer.m_hwnd;

	shared.init(hwnd);

	{
		TotalsParams params; // 初期値。
		shared.setSenderTotalsParams(&params);
		shared.setReceiverTotalsParams(&params);
	}

	m_handle = ::CreateThread(0, 0, threadProc, 0, 0, &m_tid);
	return !!m_handle;
}

BOOL SubThreadManager::exit(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubThreadManager::exit()\n"));

	shared.term();

//	return ::PostThreadMessage(m_tid, WM_QUIT, 0, 0);
	return ::TerminateThread(m_handle, -1);
}

BOOL SubThreadManager::requestCache(LPCSTR fileName)
{
	MY_TRACE(_T("SubThreadManager::requestCache(%hs)\n"), fileName);

	return ::PostThreadMessage(m_tid, SubThread::WM_POST_CACHE_REQUEST, (WPARAM)new CacheRequest(fileName), 0);
}

BOOL SubThreadManager::notifyProjectChanged(const ProjectParamsPtr& params)
{
	MY_TRACE(_T("SubThreadManager::notifyProjectChanged()\n"));

	return ::PostThreadMessage(m_tid, SubThread::WM_SEND_PROJECT_CHANGED, (WPARAM)new ProjectParams(*params), 0);
}

BOOL SubThreadManager::notifyItemChanged(const AudioParamsPtr& params)
{
	MY_TRACE(_T("SubThreadManager::notifyItemChanged()\n"));

	return ::PostThreadMessage(m_tid, SubThread::WM_SEND_ITEM_CHANGED, (WPARAM)new AudioParams(*params), 0);
}

BOOL SubThreadManager::notifyTotalsChanged(const TotalsParamsPtr& params)
{
	MY_TRACE(_T("SubThreadManager::notifyTotalsChanged()\n"));

	return ::PostThreadMessage(m_tid, SubThread::WM_SEND_TOTALS_CHANGED, (WPARAM)new TotalsParams(*params), 0);
}

BOOL SubThreadManager::requestClear()
{
	MY_TRACE(_T("SubThreadManager::requestClear()\n"));

	return ::PostThreadMessage(m_tid, SubThread::WM_POST_CLEAR_REQUEST, 0, 0);
}

BOOL SubThreadManager::requestRedraw()
{
	MY_TRACE(_T("SubThreadManager::requestRedraw()\n"));

	return ::PostThreadMessage(m_tid, SubThread::WM_POST_REDRAW_REQUEST, 0, 0);
}

//--------------------------------------------------------------------

DWORD CALLBACK SubThreadManager::threadProc(LPVOID param)
{
	SubThread subThread;

	return subThread.proc();
}

//--------------------------------------------------------------------
