#include "pch.h"
#include "SubProcess.h"
#include "App.h"

//--------------------------------------------------------------------

BOOL SubProcess::init(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubProcess::init()\n"));

	if (!m_fullSamplesContainer.init(fp))
	{
		MY_TRACE(_T("全体の音声波形ウィンドウの作成に失敗しました\n"));

		return FALSE;
	}

	TCHAR path[MAX_PATH] = {};
	::GetModuleFileName(fp->dll_hinst, path, MAX_PATH);
	::PathRemoveExtension(path);
	::PathAppend(path, _T("ShowWaveform.exe"));
	MY_TRACE_TSTR(path);

	TCHAR args[MAX_PATH] = {};
	::StringCbPrintf(args, sizeof(args), _T("0x%08p"), m_fullSamplesContainer.m_hwnd);
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

	m_fullSamplesContainer.exit(fp);

	return ::PostMessage(m_fullSamplesWindow, WM_AVIUTL_FILTER_EXIT, 0, 0);
}

//--------------------------------------------------------------------

BOOL SubProcess::Window::init(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubProcess::Window::init()\n"));

	WNDCLASSEX wc = { sizeof(wc) };
	wc.style = 0;
	wc.lpfnWndProc = wndProc;
	wc.hInstance = fp->dll_hinst;
	wc.hCursor = ::LoadCursor(0, IDC_ARROW);
	wc.lpszClassName = _T("AviUtl");

	if (!::RegisterClassEx(&wc))
		return FALSE;

	m_hwnd = ::CreateWindowEx(
		WS_EX_NOPARENTNOTIFY,
		_T("AviUtl"),
		_T("全体の音声波形"),
		WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
		0, 0, 400, 400,
		::GetWindow(fp->hwnd, GW_OWNER), 0, fp->dll_hinst, 0);

	::SetProp(m_hwnd, _T("SubProcess.Window"), this);

	return !!m_hwnd;
}

BOOL SubProcess::Window::exit(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubProcess::Window::exit()\n"));

	::DestroyWindow(m_hwnd);

	return TRUE;
}

void SubProcess::Window::delayedUpdate()
{
	MY_TRACE(_T("SubProcess::Window::delayedUpdate()\n"));

	::SetTimer(m_hwnd, TimerID, 100, 0);
}

LRESULT SubProcess::Window::onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		{
			MY_TRACE(_T("SubProcess::Window::onWndProc(WM_CLOSE, 0x%08X, 0x%08X)\n"), wParam, lParam);

			if (::IsWindowVisible(hwnd))
				::ShowWindow(hwnd, SW_HIDE);
			else
				::ShowWindow(hwnd, SW_SHOW);

			return 0;
		}
	case WM_SIZE:
		{
			MY_TRACE(_T("SubProcess::Window::onWndProc(WM_SIZE, 0x%08X, 0x%08X)\n"), wParam, lParam);

			::PostMessage(theApp.m_subProcess.m_fullSamplesWindow, WM_AVIUTL_FILTER_RESIZE, 0, 0);

			break;
		}
	case WM_TIMER:
		{
			MY_TRACE(_T("SubProcess::Window::onWndProc(WM_TIMER, 0x%08X, 0x%08X)\n"), wParam, lParam);

			if (wParam == TimerID)
			{
				if (theApp.m_subProcess.m_fullSamplesWindow)
				{
					MY_TRACE(_T("delayed update\n"));

					::KillTimer(hwnd, wParam);

					theApp.updateProjectParams();
					theApp.updateItemCache(TRUE);
				}
			}

			break;
		}
	}

	if (message == WM_AVIUTL_FILTER_INIT)
	{
		MY_TRACE(_T("SubProcess::Window::onWndProc(WM_AVIUTL_FILTER_INIT, 0x%08X, 0x%08X)\n"), wParam, lParam);

		theApp.m_subProcess.m_fullSamplesWindow = (HWND)wParam;

		::PostMessage(theApp.m_subProcess.m_fullSamplesWindow, WM_AVIUTL_FILTER_RESIZE, 0, 0);
	}
	else if (message == WM_AVIUTL_FILTER_RECEIVE)
	{
		MY_TRACE(_T("SubProcess::Window::onWndProc(WM_AVIUTL_FILTER_RECEIVE, 0x%08X, 0x%08X)\n"), wParam, lParam);

		if (!theApp.m_exists)
			return FALSE;

		theApp.m_fileCacheManager.receiveCache();
		theApp.m_itemCacheManager.update(FALSE);
	}
	else if (message == WM_AVIUTL_FILTER_CHANGE_FRAME)
	{
		MY_TRACE(_T("SubProcess::Window::onWndProc(WM_AVIUTL_FILTER_CHANGE_FRAME, 0x%08X, 0x%08X)\n"), wParam, lParam);

		int32_t frame = (int32_t)wParam;
		MY_TRACE_INT(frame);

		theApp.m_fp->exfunc->set_frame(theApp.m_auin.GetEditp(), frame);
		::PostMessage(theApp.m_fp->hwnd, AviUtl::FilterPlugin::WindowMessage::Command, 0, 0);
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK SubProcess::Window::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Window* window = (Window*)::GetProp(hwnd, _T("SubProcess.Window"));
	if (window) return window->onWndProc(hwnd, message, wParam, lParam);
	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------
