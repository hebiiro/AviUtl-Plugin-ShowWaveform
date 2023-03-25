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

	if (::GetCapture() == hwnd)
	{
		POINT point = LP2PT(lParam);
		int offset = point.y - m_dragOriginPoint.y;

		switch (m_dragMode)
		{
		case DragMode::maxRange: m_maxRange = m_dragOriginRange + offset / 5; break;
		case DragMode::minRange: m_minRange = m_dragOriginRange + offset / 5; break;
		}

		::InvalidateRect(hwnd, 0, FALSE);
	}

	{
		int c = (int)fullSamples.size();
		if (c > 0)
		{
			POINT point = LP2PT(lParam);
			RECT rc; ::GetClientRect(hwnd, &rc);
			int range = getWidth(rc) - g_design.body.margin * 2;
			int x = point.x - rc.left - g_design.body.margin;
			x = std::min<int>(x, range);
			x = std::max<int>(x, 0);

			int hotFrame = (c - 1) * x / range;
			if (m_hotFrame != hotFrame)
			{
				m_hotFrame = hotFrame;

				::InvalidateRect(hwnd, 0, FALSE);
			}
		}
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onLButtonDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onLButtonDown(0x%08X, 0x%08X)\n"), wParam, lParam);

	POINT point = LP2PT(lParam);
	RECT rc; ::GetClientRect(hwnd, &rc);

	if (point.y < rc.top + g_design.body.margin ||
		point.y > rc.bottom - g_design.body.margin)
	{
		::SetCapture(hwnd);
		m_dragOriginPoint = point;

		RECT rc; ::GetClientRect(hwnd, &rc);
		int cy = (rc.top + rc.bottom) / 2;

		if (m_dragOriginPoint.y < cy)
		{
			m_dragMode = DragMode::maxRange;
			m_dragOriginRange = m_maxRange;
		}
		else
		{
			m_dragMode = DragMode::minRange;
			m_dragOriginRange = m_minRange;
		}
	}
	else
	{
		int c = (int)fullSamples.size();
		if (c > 0)
		{
			int range = getWidth(rc) - g_design.body.margin * 2;
			int x = point.x - rc.left - g_design.body.margin;
			x = std::min<int>(x, range);
			x = std::max<int>(x, 0);

			int32_t frame = (c - 1) * x / range;

			::PostMessage(g_parent, WM_AVIUTL_FILTER_CHANGE_FRAME, (WPARAM)frame, 0);
		}
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onLButtonUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onLButtonUp(0x%08X, 0x%08X)\n"), wParam, lParam);

	::ReleaseCapture();

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------
