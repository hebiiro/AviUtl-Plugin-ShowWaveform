#include "pch.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

LRESULT MainWindow::onSetCursor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	MY_TRACE(_T("MainWindow::onSetCursor(0x%08X, 0x%08X)\n"), wParam, lParam);

	HWND cursorHolder = (HWND)wParam;

	if (cursorHolder == hwnd && LOWORD(lParam) == HTCLIENT)
	{
		POINT point; ::GetCursorPos(&point);
		::MapWindowPoints(0, hwnd, &point, 1);
		RECT rc; ::GetClientRect(hwnd, &rc);

		if (point.y < rc.top + g_design.body.margin ||
			point.y > rc.bottom - g_design.body.margin)
		{
			::SetCursor(::LoadCursor(0, IDC_SIZENS));
			return TRUE;
		}
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onMouseMove(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	MY_TRACE(_T("MainWindow::onMouseMove(0x%08X, 0x%08X)\n"), wParam, lParam);

	return m_mode->onMouseMove(*this, hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onLButtonDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onLButtonDown(0x%08X, 0x%08X)\n"), wParam, lParam);

	return m_mode->onLButtonDown(*this, hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onLButtonUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onLButtonUp(0x%08X, 0x%08X)\n"), wParam, lParam);

	return m_mode->onLButtonUp(*this, hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------
