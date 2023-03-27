#include "pch.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

void MainWindow::recalcLayout()
{
	MY_TRACE(_T("MainWindow::recalcLayout()\n"));

	RECT rc; ::GetClientRect(m_hwnd, &rc);
	int width = getWidth(rc);
	width -= g_design.body.margin * 2;

	int logWidth = width * (100 + m_zoom) / 100;

	SCROLLINFO si = { sizeof(si) };
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = logWidth;
	si.nPage = width + 1;
	::SetScrollInfo(m_hwnd, SB_HORZ, &si, TRUE);
}

int MainWindow::px2Frame(int x)
{
	int c = (int)fullSamples.size();
	if (c <= 1) return 0;

	RECT rc; ::GetClientRect(m_hwnd, &rc);
	int hScroll = ::GetScrollPos(m_hwnd, SB_HORZ);
	int width = getWidth(rc) - g_design.body.margin * 2;
	int range = width * (100 + m_zoom) / 100;
	x = x + hScroll - rc.left - g_design.body.margin;
	x = std::min<int>(x, range);
	x = std::max<int>(x, 0);

	return (c - 1) * x / range;
}

//--------------------------------------------------------------------

LRESULT MainWindow::onSize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onSize(0x%08X, 0x%08X)\n"), wParam, lParam);

	recalcLayout();

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onHScroll(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UINT sb = LOWORD(wParam);
	int thumbPos = HIWORD(wParam);
	HWND scrollBar = (HWND)lParam;

	MY_TRACE(_T("MainWindow::onHScroll(%d, %d, 0x%08X)\n"), sb, thumbPos, scrollBar);

	SCROLLINFO si = { sizeof(si) };
	si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	::GetScrollInfo(hwnd, SB_HORZ, &si);

	int pos = si.nPos;

	switch (sb)
	{
	case SB_LEFT: pos = si.nMin; break;
	case SB_RIGHT: pos = si.nMax; break;
	case SB_LINELEFT: pos = si.nPos - 1; break;
	case SB_LINERIGHT: pos = si.nPos + 1; break;
	case SB_PAGELEFT: pos = si.nPos - 100; break;
	case SB_PAGERIGHT: pos = si.nPos + 100; break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK: pos = thumbPos; break;
	}

	int min = 0;
	int max = si.nMax - std::max<int>(si.nPage - 1, 0);

	pos = std::max<int>(pos, min);
	pos = std::min<int>(pos, max);

	if (si.nPos != pos)
	{
		si.nPos = pos;
		::SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		::InvalidateRect(hwnd, 0, FALSE);

		return TRUE;
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onMouseWheel(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int delta = (short)HIWORD(wParam);
	POINT point = LP2PT(lParam);
	::MapWindowPoints(0, hwnd, &point, 1);

	MY_TRACE(_T("MainWindow::onMouseWheel(%d, %d, %d)\n"), delta, point.x, point.y);

	if (delta > 0)
	{
		m_zoom += 10;
	}
	else
	{
		m_zoom -= 10;
	}

	m_zoom = std::max<int>(m_zoom, 0);

	recalcLayout();

	::InvalidateRect(hwnd, 0, FALSE);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------
