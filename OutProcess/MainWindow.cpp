#include "pch.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

MainWindow g_mainWindow;

//--------------------------------------------------------------------

LRESULT MainWindow::onCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onCreate()\n"));

	m_hwnd = hwnd;

	if (!initOpenGL()) return -1;
	if (!initWaveform()) return -1;
	if (!initConfig()) return -1;

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onDestroy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onDestroy()\n"));

	termConfig();
	termWaveform();
	termOpenGL();

	::PostQuitMessage(0);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onTimer(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	MY_TRACE(_T("MainWindow::onTimer()\n"));

	switch (wParam)
	{
	case TimerID::checkConfig: return onTimerCheckConfig(hwnd, message, wParam, lParam);
	}

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
		// 音声波形を再計算する。
		recalcWaveform();
	}

	return 0;
}

LRESULT CALLBACK MainWindow::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE: return g_mainWindow.onCreate(hwnd, message, wParam, lParam);
	case WM_DESTROY: return g_mainWindow.onDestroy(hwnd, message, wParam, lParam);
	case WM_TIMER: return g_mainWindow.onTimer(hwnd, message, wParam, lParam);
	case WM_PAINT: return g_mainWindow.onPaint(hwnd, message, wParam, lParam);
	case WM_SETCURSOR: return g_mainWindow.onSetCursor(hwnd, message, wParam, lParam);
	case WM_MOUSEMOVE: return g_mainWindow.onMouseMove(hwnd, message, wParam, lParam);
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
