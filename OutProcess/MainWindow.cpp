#include "pch.h"
#include "MainWindow.h"
#include "App.h"

//--------------------------------------------------------------------

BOOL MainWindow::getShowBPM()
{
	return m_fullSamplesParams.showBPM;
}

BOOL MainWindow::setShowBPM(BOOL showBPM)
{
	if (m_fullSamplesParams.showBPM == showBPM)
		return FALSE;

	m_fullSamplesParams.showBPM = showBPM;
	redraw();
	updateShared();

	return TRUE;
}

int MainWindow::getOrig()
{
	return m_fullSamplesParams.tempo.orig;
}

BOOL MainWindow::setOrig(int orig)
{
	if (m_fullSamplesParams.tempo.orig == orig)
		return FALSE;

	m_fullSamplesParams.tempo.orig = orig;
	redraw();
	updateShared();

	return TRUE;
}

int MainWindow::getBPM()
{
	return m_fullSamplesParams.tempo.bpm;
}

BOOL MainWindow::setBPM(int bpm)
{
	if (m_fullSamplesParams.tempo.bpm == bpm)
		return FALSE;

	m_fullSamplesParams.tempo.bpm = bpm;
	redraw();
	updateShared();

	return TRUE;
}

int MainWindow::getAbove()
{
	return m_fullSamplesParams.tempo.above;
}

BOOL MainWindow::setAbove(int above)
{
	above = std::max(1, above);

	if (m_fullSamplesParams.tempo.above == above)
		return FALSE;

	m_fullSamplesParams.tempo.above = above;
	redraw();
	updateShared();

	return TRUE;
}

int MainWindow::getBelow()
{
	return m_fullSamplesParams.tempo.below;
}

BOOL MainWindow::setBelow(int below)
{
	below = std::max(1, below);

	if (m_fullSamplesParams.tempo.below == below)
		return FALSE;

	m_fullSamplesParams.tempo.below = below;
	redraw();
	updateShared();

	return TRUE;
}

int MainWindow::getLimitVolume()
{
	return m_limitVolume;
}

BOOL MainWindow::setLimitVolume(int newLimitVolume)
{
	newLimitVolume = std::max(1, newLimitVolume);
	newLimitVolume = std::min(100, newLimitVolume);

	if (m_limitVolume == newLimitVolume)
		return FALSE;

	m_limitVolume = newLimitVolume;
	redraw();

	return TRUE;
}

int MainWindow::getBaseVolume()
{
	return m_baseVolume;
}

BOOL MainWindow::setBaseVolume(int newBaseVolume)
{
	if (m_baseVolume == newBaseVolume)
		return FALSE;

	m_baseVolume = newBaseVolume;
	redraw();

	return TRUE;
}

int MainWindow::getMinRMS()
{
	return m_minRMS;
}

BOOL MainWindow::setMinRMS(int newMinRMS)
{
	if (m_minRMS == newMinRMS)
		return FALSE;

	m_minRMS = newMinRMS;
	redraw();

	return TRUE;
}

int MainWindow::getMaxRMS()
{
	return m_maxRMS;
}

BOOL MainWindow::setMaxRMS(int newMaxRMS)
{
	if (m_maxRMS == newMaxRMS)
		return FALSE;

	m_maxRMS = newMaxRMS;
	redraw();

	return TRUE;
}

int MainWindow::getBaseRMS()
{
	return m_baseRMS;
}

BOOL MainWindow::setBaseRMS(int newBaseRMS)
{
	if (m_baseRMS == newBaseRMS)
		return FALSE;

	m_baseRMS = newBaseRMS;
	redraw();

	return TRUE;
}

int MainWindow::getZoom()
{
	return m_zoom;
}

BOOL MainWindow::setZoom(int newZoom)
{
	if (m_zoom == newZoom)
		return FALSE;

	m_zoom = newZoom;
	recalcLayout();
	redraw();

	return TRUE;
}

ModePtr MainWindow::getMode()
{
	return m_mode;
}

int MainWindow::getHotFrame()
{
	return m_hotFrame;
}

BOOL MainWindow::setHotFrame(int newHotFrame)
{
	if (m_hotFrame == newHotFrame)
		return FALSE;

	m_hotFrame = newHotFrame;
	redraw();

	return TRUE;
}

BOOL MainWindow::updateShared()
{
	MY_TRACE(_T("MainWindow::updateShared()\n"));

	return shared.setReceiverFullSamplesParams(&m_fullSamplesParams);
}

//--------------------------------------------------------------------

BOOL MainWindow::create(HINSTANCE instance, HWND parent)
{
	MY_TRACE(_T("MainWindow::create(0x%08X, 0x%08X)\n"), instance, parent);

	const LPCWSTR CLASS_NAME = L"ShowWaveform.OutProcess.Window";

	WNDCLASSEXW wc = { sizeof(wc) };
	wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc    = wndProc;
	wc.hInstance      = instance;
	wc.hCursor        = ::LoadCursor(0, IDC_ARROW);
	wc.lpszClassName  = CLASS_NAME;

	if (!::RegisterClassExW(&wc))
		return FALSE;

	::CreateWindowExW(
		WS_EX_NOPARENTNOTIFY,
		CLASS_NAME,
		CLASS_NAME,
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, 0, 0,
		parent, 0, instance, 0);

	if (!theApp.mainWindow.m_hwnd)
		return FALSE;

	return TRUE;
}

void MainWindow::redraw()
{
	::InvalidateRect(*this, 0, FALSE);
}

//--------------------------------------------------------------------

LRESULT MainWindow::onCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onCreate(0x%08X, 0x%08X)\n"), wParam, lParam);

	m_hwnd = hwnd;

	{
		// DarkenWindow が存在する場合は読み込む。

		TCHAR fileName[MAX_PATH] = {};
		::GetModuleFileName(theApp.instance, fileName, MAX_PATH);
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
	if (!initConfig()) return -1;

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onDestroy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onDestroy(0x%08X, 0x%08X)\n"), wParam, lParam);

	termConfig();
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

	::AppendMenu(menu, MF_STRING, CommandID::Mode::rms, _T("RMS"));
	::AppendMenu(menu, MF_STRING, CommandID::Mode::center, _T("中央"));
	::AppendMenu(menu, MF_STRING, CommandID::Mode::bottomUp, _T("ボトムアップ"));
	::AppendMenu(menu, MF_STRING, CommandID::Mode::topDown, _T("トップダウン"));
	::AppendMenu(menu, MF_SEPARATOR, 0, 0);
	::AppendMenu(menu, MF_STRING, CommandID::HorzScale::VisibleStyle::both, _T("水平目盛りを上下に表示"));
	::AppendMenu(menu, MF_STRING, CommandID::HorzScale::VisibleStyle::top, _T("水平目盛りを上のみに表示"));
	::AppendMenu(menu, MF_STRING, CommandID::HorzScale::VisibleStyle::bottom, _T("水平目盛りを下のみに表示"));
	::AppendMenu(menu, MF_SEPARATOR, 0, 0);
	::AppendMenu(menu, MF_STRING, CommandID::HorzScale::LineStyle::side, _T("水平目盛りをサイドだけに表示"));
	::AppendMenu(menu, MF_STRING, CommandID::HorzScale::LineStyle::straight, _T("水平目盛りを一直線に表示"));
	::AppendMenu(menu, MF_SEPARATOR, 0, 0);
	::AppendMenu(menu, MF_STRING, CommandID::VertScale::VisibleStyle::both, _T("垂直目盛りを左右に表示"));
	::AppendMenu(menu, MF_STRING, CommandID::VertScale::VisibleStyle::left, _T("垂直目盛りを左のみに表示"));
	::AppendMenu(menu, MF_STRING, CommandID::VertScale::VisibleStyle::right, _T("垂直目盛りを右のみに表示"));
	::AppendMenu(menu, MF_SEPARATOR, 0, 0);
	::AppendMenu(menu, MF_STRING, CommandID::VertScale::LineStyle::side, _T("垂直目盛りをサイドだけに表示"));
	::AppendMenu(menu, MF_STRING, CommandID::VertScale::LineStyle::straight, _T("垂直目盛りを一直線に表示"));
	::AppendMenu(menu, MF_SEPARATOR, 0, 0);
	::AppendMenu(menu, MF_STRING, CommandID::showMainDialog, _T("その他の設定"));

	switch (m_mode->getID())
	{
	case Mode::rms: ::CheckMenuItem(menu, CommandID::Mode::rms, MF_CHECKED); break;
	case Mode::center: ::CheckMenuItem(menu, CommandID::Mode::center, MF_CHECKED); break;
	case Mode::bottomUp: ::CheckMenuItem(menu, CommandID::Mode::bottomUp, MF_CHECKED); break;
	case Mode::topDown: ::CheckMenuItem(menu, CommandID::Mode::topDown, MF_CHECKED); break;
	}

	switch (m_horzScaleSettings.visibleStyle)
	{
	case HorzScaleSettings::VisibleStyle::both: ::CheckMenuItem(menu, CommandID::HorzScale::VisibleStyle::both, MF_CHECKED); break;
	case HorzScaleSettings::VisibleStyle::top: ::CheckMenuItem(menu, CommandID::HorzScale::VisibleStyle::top, MF_CHECKED); break;
	case HorzScaleSettings::VisibleStyle::bottom: ::CheckMenuItem(menu, CommandID::HorzScale::VisibleStyle::bottom, MF_CHECKED); break;
	}

	switch (m_horzScaleSettings.lineStyle)
	{
	case HorzScaleSettings::LineStyle::side: ::CheckMenuItem(menu, CommandID::HorzScale::LineStyle::side, MF_CHECKED); break;
	case HorzScaleSettings::LineStyle::straight: ::CheckMenuItem(menu, CommandID::HorzScale::LineStyle::straight, MF_CHECKED); break;
	}

	switch (m_vertScaleSettings.visibleStyle)
	{
	case VertScaleSettings::VisibleStyle::both: ::CheckMenuItem(menu, CommandID::VertScale::VisibleStyle::both, MF_CHECKED); break;
	case VertScaleSettings::VisibleStyle::left: ::CheckMenuItem(menu, CommandID::VertScale::VisibleStyle::left, MF_CHECKED); break;
	case VertScaleSettings::VisibleStyle::right: ::CheckMenuItem(menu, CommandID::VertScale::VisibleStyle::right, MF_CHECKED); break;
	}

	switch (m_vertScaleSettings.lineStyle)
	{
	case VertScaleSettings::LineStyle::side: ::CheckMenuItem(menu, CommandID::VertScale::LineStyle::side, MF_CHECKED); break;
	case VertScaleSettings::LineStyle::straight: ::CheckMenuItem(menu, CommandID::VertScale::LineStyle::straight, MF_CHECKED); break;
	}

	int id = ::TrackPopupMenu(menu, TPM_NONOTIFY | TPM_RETURNCMD, cursorPos.x, cursorPos.y, 0, hwnd, 0);

	switch (id)
	{
	case CommandID::Mode::rms: m_mode = std::make_shared<RMSMode>(); break;
	case CommandID::Mode::center: m_mode = std::make_shared<CenterMode>(); break;
	case CommandID::Mode::bottomUp: m_mode = std::make_shared<BottomUpMode>(); break;
	case CommandID::Mode::topDown: m_mode = std::make_shared<TopDownMode>(); break;

	case CommandID::HorzScale::VisibleStyle::both: m_horzScaleSettings.visibleStyle = HorzScaleSettings::VisibleStyle::both; break;
	case CommandID::HorzScale::VisibleStyle::top: m_horzScaleSettings.visibleStyle = HorzScaleSettings::VisibleStyle::top; break;
	case CommandID::HorzScale::VisibleStyle::bottom: m_horzScaleSettings.visibleStyle = HorzScaleSettings::VisibleStyle::bottom; break;

	case CommandID::HorzScale::LineStyle::side: m_horzScaleSettings.lineStyle = HorzScaleSettings::LineStyle::side; break;
	case CommandID::HorzScale::LineStyle::straight: m_horzScaleSettings.lineStyle = HorzScaleSettings::LineStyle::straight; break;

	case CommandID::VertScale::VisibleStyle::both: m_vertScaleSettings.visibleStyle = VertScaleSettings::VisibleStyle::both; break;
	case CommandID::VertScale::VisibleStyle::left: m_vertScaleSettings.visibleStyle = VertScaleSettings::VisibleStyle::left; break;
	case CommandID::VertScale::VisibleStyle::right: m_vertScaleSettings.visibleStyle = VertScaleSettings::VisibleStyle::right; break;

	case CommandID::VertScale::LineStyle::side: m_vertScaleSettings.lineStyle = VertScaleSettings::LineStyle::side; break;
	case CommandID::VertScale::LineStyle::straight: m_vertScaleSettings.lineStyle = VertScaleSettings::LineStyle::straight; break;

	case CommandID::showMainDialog:
		{
			// メインダイアログを表示する。
			// メインダイアログはコンテナの中に入っているので、コンテナを表示状態にする。

			HWND container = ::GetParent(theApp.mainDialog);
			::ShowWindow(container, SW_SHOW);

			break;
		}
	}

	::DestroyMenu(menu);

	recalcLayout();
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

LRESULT CALLBACK MainWindow::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE: return theApp.mainWindow.onCreate(hwnd, message, wParam, lParam);
	case WM_DESTROY: return theApp.mainWindow.onDestroy(hwnd, message, wParam, lParam);
	case WM_SIZE: return theApp.mainWindow.onSize(hwnd, message, wParam, lParam);
	case WM_HSCROLL: return theApp.mainWindow.onHScroll(hwnd, message, wParam, lParam);
	case WM_TIMER: return theApp.mainWindow.onTimer(hwnd, message, wParam, lParam);
	case WM_PAINT: return theApp.mainWindow.onPaint(hwnd, message, wParam, lParam);
	case WM_CONTEXTMENU: return theApp.mainWindow.onContextMenu(hwnd, message, wParam, lParam);
	case WM_SETCURSOR: return theApp.mainWindow.onSetCursor(hwnd, message, wParam, lParam);
	case WM_MOUSEMOVE: return theApp.mainWindow.onMouseMove(hwnd, message, wParam, lParam);
	case WM_MOUSEWHEEL: return theApp.mainWindow.onMouseWheel(hwnd, message, wParam, lParam);
	case WM_LBUTTONDOWN: return theApp.mainWindow.onLButtonDown(hwnd, message, wParam, lParam);
	case WM_LBUTTONUP: return theApp.mainWindow.onLButtonUp(hwnd, message, wParam, lParam);
	}

	if (message == WM_AVIUTL_FILTER_EXIT) return theApp.mainWindow.onAviUtlFilterExit(hwnd, message, wParam, lParam);
	else if (message == WM_AVIUTL_FILTER_RESIZE) return theApp.mainWindow.onAviUtlFilterResize(hwnd, message, wParam, lParam);
	else if (message == WM_AVIUTL_FILTER_SEND) return theApp.onAviUtlFilterSend(hwnd, message, wParam, lParam);
	else if (message == WM_AVIUTL_FILTER_RECEIVE) return theApp.onAviUtlFilterReceive(hwnd, message, wParam, lParam);
	else if (message == WM_AVIUTL_FILTER_CLEAR) return theApp.onAviUtlFilterClear(hwnd, message, wParam, lParam);
	else if (message == WM_AVIUTL_FILTER_REDRAW) return theApp.onAviUtlFilterRedraw(hwnd, message, wParam, lParam);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------
