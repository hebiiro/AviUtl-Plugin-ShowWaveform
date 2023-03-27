#include "pch.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

MainWindow g_mainWindow;

//--------------------------------------------------------------------

LRESULT MainWindow::onCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onCreate(0x%08X, 0x%08X)\n"), wParam, lParam);

	m_hwnd = hwnd;

	{
		// DarkenWindow が存在する場合は読み込む。

		TCHAR fileName[MAX_PATH] = {};
		::GetModuleFileName(g_instance, fileName, MAX_PATH);
		::PathRemoveFileSpec(fileName);
		::PathAppend(fileName, _T("..\\DarkenWindow.aul"));
		MY_TRACE_TSTR(fileName);

		HMODULE DarkenWindow = ::LoadLibrary(fileName);
		MY_TRACE_HEX(DarkenWindow);

		if (DarkenWindow)
		{
			typedef void (WINAPI* Type_DarkenWindow_init)(HWND hwnd);
			Type_DarkenWindow_init DarkenWindow_init =
				(Type_DarkenWindow_init)::GetProcAddress(DarkenWindow, "DarkenWindow_init");
			MY_TRACE_HEX(DarkenWindow_init);

			if (DarkenWindow_init)
				DarkenWindow_init(hwnd);
		}
	}

	if (!initOpenGL()) return -1;
	if (!initWaveform()) return -1;
	if (!initConfig()) return -1;

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onDestroy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onDestroy(0x%08X, 0x%08X)\n"), wParam, lParam);

	termConfig();
	termWaveform();
	termOpenGL();

	::PostQuitMessage(0);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onTimer(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	MY_TRACE(_T("MainWindow::onTimer(0x%08X, 0x%08X)\n"), wParam, lParam);

	switch (wParam)
	{
	case TimerID::checkConfig: return onTimerCheckConfig(hwnd, message, wParam, lParam);
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onContextMenu(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	MY_TRACE(_T("MainWindow::onContextMenu(0x%08X, 0x%08X)\n"), wParam, lParam);

	POINT cursorPos; ::GetCursorPos(&cursorPos);

	HMENU menu = ::CreatePopupMenu();

	::AppendMenu(menu, MF_STRING, CommandID::rmsMode, _T("RMS"));
	::AppendMenu(menu, MF_STRING, CommandID::bothMode, _T("中央"));
	::AppendMenu(menu, MF_STRING, CommandID::minMode, _T("下側"));
	::AppendMenu(menu, MF_STRING, CommandID::maxMode, _T("上側"));

	switch (m_mode->getID())
	{
	case Mode::rms: ::CheckMenuItem(menu, CommandID::rmsMode, MF_CHECKED); break;
	case Mode::both: ::CheckMenuItem(menu, CommandID::bothMode, MF_CHECKED); break;
	case Mode::min: ::CheckMenuItem(menu, CommandID::minMode, MF_CHECKED); break;
	case Mode::max: ::CheckMenuItem(menu, CommandID::maxMode, MF_CHECKED); break;
	}

	int id = ::TrackPopupMenu(menu, TPM_NONOTIFY | TPM_RETURNCMD, cursorPos.x, cursorPos.y, 0, hwnd, 0);

	switch (id)
	{
	case CommandID::rmsMode: m_mode = std::make_shared<RMSMode>(); break;
	case CommandID::bothMode: m_mode = std::make_shared<BothMode>(); break;
	case CommandID::minMode: m_mode = std::make_shared<MinMode>(); break;
	case CommandID::maxMode: m_mode = std::make_shared<MaxMode>(); break;
	}

	::DestroyMenu(menu);

	::InvalidateRect(hwnd, 0, FALSE);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onAviUtlFilterExit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterExit(0x%08X, 0x%08X)\n"), wParam, lParam);

	::PostQuitMessage(0);

	return 0;
}

LRESULT MainWindow::onAviUtlFilterResize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterResize(0x%08X, 0x%08X)\n"), wParam, lParam);

	HWND parent = ::GetParent(hwnd);
	RECT rc; ::GetClientRect(parent, &rc);
	MY_TRACE_RECT2(rc);
	::SetWindowPos(hwnd, 0, rc.left, rc.top, getWidth(rc), getHeight(rc), SWP_NOZORDER);

	return 0;
}

LRESULT MainWindow::onAviUtlFilterSend(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterSend(0x%08X, 0x%08X)\n"), wParam, lParam);

	switch (wParam)
	{
	case SendID::requestCache:
		{
			SenderBottlePtr bottle = getSenderBottle();
			if (bottle)
			{
				CachePtr cache = getCache(bottle->fileName);
				if (cache)
				{
					// キャッシュが作成済みならメインプロセスに通知する。
					sendCache(cache);
				}
				else
				{
					// キャッシュが存在しないので新規作成する。
					// (1) ファイル名からウェイターを作成する。
					// (2) ウェイターからリーダーを作成する。
					// (3) リーダーからキャッシュを作成する。

					// ウェイターを作成する。
					WaiterPtr waiter = createWaiter(bottle->fileName);

					// 可能であれば、ウェイターを消化する。
					digestWaiterQueue();
				}
			}

			break;
		}
	case SendID::notifyProjectChanged:
		{
			ProjectParamsPtr params = getProjectParams();
			if (params)
				setProjectParams(params);

			break;
		}
	case SendID::notifyItemChanged:
		{
			AudioParamsPtr params = getAudioParams();
			if (params)
				setAudioParams(params);

			break;
		}
	}

	return 0;
}

LRESULT MainWindow::onAviUtlFilterReceive(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterReceive(0x%08X, 0x%08X)\n"), wParam, lParam);

	DWORD id = (DWORD)wParam;
	MY_TRACE_INT(id);

	// リーダーを取得する。
	ReaderPtr reader = getReader(id);
	if (reader)
	{
		// キャッシュを作成する。
		CachePtr cache = createCache(reader);
		if (cache)
		{
			// キャッシュを作成できた場合はメインプロセスに通知する。
			sendCache(cache);

			// このリーダーは不要になったので削除する。
			eraseReader(id);

			// リーダーの空きができたので次の読み込みを開始する。
			digestWaiterQueue();
		}
	}

	return 0;
}

LRESULT MainWindow::onAviUtlFilterClear(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterClear(0x%08X, 0x%08X)\n"), wParam, lParam);

	cacheMap.clear();

	return 0;
}

LRESULT MainWindow::onAviUtlFilterRedraw(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterRedraw(0x%08X, 0x%08X)\n"), wParam, lParam);

	if (::IsWindowVisible(m_hwnd))
	{
		// 全体の音声波形を再計算する。
		recalcFullSamples();
	}

	return 0;
}

LRESULT CALLBACK MainWindow::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE: return g_mainWindow.onCreate(hwnd, message, wParam, lParam);
	case WM_DESTROY: return g_mainWindow.onDestroy(hwnd, message, wParam, lParam);
	case WM_SIZE: return g_mainWindow.onSize(hwnd, message, wParam, lParam);
	case WM_HSCROLL: return g_mainWindow.onHScroll(hwnd, message, wParam, lParam);
	case WM_TIMER: return g_mainWindow.onTimer(hwnd, message, wParam, lParam);
	case WM_PAINT: return g_mainWindow.onPaint(hwnd, message, wParam, lParam);
	case WM_CONTEXTMENU: return g_mainWindow.onContextMenu(hwnd, message, wParam, lParam);
	case WM_SETCURSOR: return g_mainWindow.onSetCursor(hwnd, message, wParam, lParam);
	case WM_MOUSEMOVE: return g_mainWindow.onMouseMove(hwnd, message, wParam, lParam);
	case WM_MOUSEWHEEL: return g_mainWindow.onMouseWheel(hwnd, message, wParam, lParam);
	case WM_LBUTTONDOWN: return g_mainWindow.onLButtonDown(hwnd, message, wParam, lParam);
	case WM_LBUTTONUP: return g_mainWindow.onLButtonUp(hwnd, message, wParam, lParam);
	}

	if (message == WM_AVIUTL_FILTER_EXIT) return g_mainWindow.onAviUtlFilterExit(hwnd, message, wParam, lParam);
	else if (message == WM_AVIUTL_FILTER_RESIZE) return g_mainWindow.onAviUtlFilterResize(hwnd, message, wParam, lParam);
	else if (message == WM_AVIUTL_FILTER_SEND) return g_mainWindow.onAviUtlFilterSend(hwnd, message, wParam, lParam);
	else if (message == WM_AVIUTL_FILTER_RECEIVE) return g_mainWindow.onAviUtlFilterReceive(hwnd, message, wParam, lParam);
	else if (message == WM_AVIUTL_FILTER_CLEAR) return g_mainWindow.onAviUtlFilterClear(hwnd, message, wParam, lParam);
	else if (message == WM_AVIUTL_FILTER_REDRAW) return g_mainWindow.onAviUtlFilterRedraw(hwnd, message, wParam, lParam);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------
