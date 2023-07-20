#include "pch.h"
#include "SubProcess.h"
#include "App.h"

//--------------------------------------------------------------------

BOOL SubProcess::init(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubProcess::init()\n"));

	// サブプロセスのウィンドウの親となるコンテナウィンドウを作成する。

	HWND owner = ::GetWindow(fp->hwnd, GW_OWNER);

	if (!m_windowContainer.init(fp, owner, _T("全体の音声波形")))
	{
		MY_TRACE(_T("全体の音声波形ウィンドウの作成に失敗しました\n"));

		return FALSE;
	}

	if (!m_dialogContainer.init(fp, owner, _T("全体の音声波形の設定")))
	{
		MY_TRACE(_T("全体の音声波形ダイアログの作成に失敗しました\n"));

		return FALSE;
	}

	// ウィンドウコンテナとダイアログコンテナを関連付ける。

	::SetProp(m_windowContainer.m_hwnd, PROP_NAME_DIALOG_CONTAINER, m_dialogContainer.m_hwnd);
	::SetProp(m_dialogContainer.m_hwnd, PROP_NAME_WINDOW_CONTAINER, m_windowContainer.m_hwnd);

	// コンテナウィンドウの作成が完了したので、サブプロセスを起動する。

	TCHAR path[MAX_PATH] = {};
	::GetModuleFileName(fp->dll_hinst, path, MAX_PATH);
	::PathRemoveExtension(path);
	::PathAppend(path, _T("ShowWaveform.exe"));
	MY_TRACE_TSTR(path);

	TCHAR args[MAX_PATH] = {};
	::StringCbPrintf(args, sizeof(args), _T("0x%08p"), m_windowContainer.m_hwnd);
	MY_TRACE_TSTR(args);

	STARTUPINFO si = { sizeof(si) };
	if (!::CreateProcess(
		path,           // No module name (use command line)
		args,           // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&m_pi))         // Pointer to PROCESS_INFORMATION structur
	{
		MY_TRACE(_T("::CreateProcess() failed.\n"));

		return FALSE;
	}

	return TRUE;
}

BOOL SubProcess::exit(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubProcess::exit()\n"));

	m_dialogContainer.exit(fp);
	m_windowContainer.exit(fp);

	::PostMessage(m_dialogContainer.m_inner, WM_AVIUTL_FILTER_EXIT, 0, 0);
	::PostMessage(m_windowContainer.m_inner, WM_AVIUTL_FILTER_EXIT, 0, 0);

	return TRUE;
}

//--------------------------------------------------------------------

BOOL SubProcess::Container::init(AviUtl::FilterPlugin* fp, HWND owner, LPCTSTR windowText)
{
	MY_TRACE(_T("SubProcess::Container::init(%s)\n"), windowText);

	const LPCTSTR className = _T("AviUtl");

	WNDCLASSEX wc = { sizeof(wc) };
	wc.style = 0;
	wc.lpfnWndProc = wndProc;
	wc.hInstance = fp->dll_hinst;
	wc.hCursor = ::LoadCursor(0, IDC_ARROW);
	wc.lpszClassName = className;
	::RegisterClassEx(&wc);

	m_hwnd = ::CreateWindowEx(
		WS_EX_NOPARENTNOTIFY,
		className, windowText,
		WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
		0, 0, 400, 400,
		owner, 0, fp->dll_hinst, 0);

	::SetProp(m_hwnd, getPropName(), this);

	return !!m_hwnd;
}

BOOL SubProcess::Container::exit(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubProcess::Container::exit()\n"));

	::DestroyWindow(m_hwnd);

	return TRUE;
}

LRESULT SubProcess::Container::onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		{
			MY_TRACE(_T("SubProcess::Container::onWndProc(WM_CLOSE, 0x%08X, 0x%08X)\n"), wParam, lParam);

			if (::IsWindowVisible(hwnd))
				::ShowWindow(hwnd, SW_HIDE);
			else
				::ShowWindow(hwnd, SW_SHOW);

			return 0;
		}
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK SubProcess::Container::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Container* container = (Container*)::GetProp(hwnd, getPropName());
	if (container) return container->onWndProc(hwnd, message, wParam, lParam);
	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------

void SubProcess::WindowContainer::delayedUpdate()
{
	MY_TRACE(_T("SubProcess::WindowContainer::delayedUpdate()\n"));

	::SetTimer(m_hwnd, TimerID::Update, 100, 0);
}

void SubProcess::WindowContainer::delayedSendTotalsParams(const TotalsParams* params)
{
	MY_TRACE(_T("SubProcess::WindowContainer::delayedSendTotalsParams()\n"));

	m_delayedTotalsParams = *params;

	::SetTimer(m_hwnd, TimerID::SendTotalsParams, 100, 0);
}

LRESULT SubProcess::WindowContainer::onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		{
			MY_TRACE(_T("SubProcess::WindowContainer::onWndProc(WM_SIZE, 0x%08X, 0x%08X)\n"), wParam, lParam);

			::PostMessage(m_inner, WM_AVIUTL_FILTER_RESIZE, 0, 0);

			break;
		}
	case WM_TIMER:
		{
			MY_TRACE(_T("SubProcess::WindowContainer::onWndProc(WM_TIMER, 0x%08X, 0x%08X)\n"), wParam, lParam);

			switch (wParam)
			{
			case TimerID::Update:
				{
					if (m_inner)
					{
						MY_TRACE(_T("delayed update\n"));

						::KillTimer(hwnd, TimerID::Update);

						theApp.updateProjectParams();
						theApp.updateItemCache(TRUE);
					}

					break;
				}
			case TimerID::SendTotalsParams:
				{
					if (m_inner)
					{
						MY_TRACE(_T("delayed send totals params\n"));

						::KillTimer(hwnd, TimerID::SendTotalsParams);

						theApp.sendTotalsParams(&m_delayedTotalsParams);
					}

					break;
				}
			}

			break;
		}
	}

	if (message == WM_AVIUTL_FILTER_INIT)
	{
		MY_TRACE(_T("SubProcess::WindowContainer::onWndProc(WM_AVIUTL_FILTER_INIT, 0x%08X, 0x%08X)\n"), wParam, lParam);

		m_inner = (HWND)wParam;
		::PostMessage(m_inner, WM_AVIUTL_FILTER_RESIZE, 0, 0);
	}
	else if (message == WM_AVIUTL_FILTER_RECEIVE)
	{
		MY_TRACE(_T("SubProcess::WindowContainer::onWndProc(WM_AVIUTL_FILTER_RECEIVE, 0x%08X, 0x%08X)\n"), wParam, lParam);

		if (!theApp.m_exists)
			return FALSE;

		theApp.m_fileCacheManager.receiveCache();
		theApp.m_itemCacheManager.update(FALSE);
	}
	else if (message == WM_AVIUTL_FILTER_CHANGE_FRAME)
	{
		MY_TRACE(_T("SubProcess::WindowContainer::onWndProc(WM_AVIUTL_FILTER_CHANGE_FRAME, 0x%08X, 0x%08X)\n"), wParam, lParam);

		int32_t frame = (int32_t)wParam;
		MY_TRACE_INT(frame);

		theApp.m_fp->exfunc->set_frame(theApp.m_auin.GetEditp(), frame);
		::PostMessage(theApp.m_fp->hwnd, AviUtl::FilterPlugin::WindowMessage::Command, 0, 0);
	}

	return Container::onWndProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------

LRESULT SubProcess::DialogContainer::onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		{
			MY_TRACE(_T("SubProcess::DialogContainer::onWndProc(WM_SIZE, 0x%08X, 0x%08X)\n"), wParam, lParam);

			::PostMessage(m_inner, WM_AVIUTL_FILTER_RESIZE, 0, 0);

			break;
		}
	}

	if (message == WM_AVIUTL_FILTER_INIT)
	{
		MY_TRACE(_T("SubProcess::DialogContainer::onWndProc(WM_AVIUTL_FILTER_INIT, 0x%08X, 0x%08X)\n"), wParam, lParam);

		m_inner = (HWND)wParam;
		::PostMessage(m_inner, WM_AVIUTL_FILTER_RESIZE, 0, 0);
	}
	else if (message == WM_AVIUTL_FILTER_RECEIVE)
	{
		MY_TRACE(_T("SubProcess::DialogContainer::onWndProc(WM_AVIUTL_FILTER_RECEIVE, 0x%08X, 0x%08X)\n"), wParam, lParam);
	}
	else if (message == WM_AVIUTL_FILTER_CHANGE_FRAME)
	{
		MY_TRACE(_T("SubProcess::DialogContainer::onWndProc(WM_AVIUTL_FILTER_CHANGE_FRAME, 0x%08X, 0x%08X)\n"), wParam, lParam);
	}

	return Container::onWndProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------
