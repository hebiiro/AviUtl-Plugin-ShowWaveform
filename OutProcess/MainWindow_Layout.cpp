#include "pch.h"
#include "MainWindow.h"
#include "App.h"

//--------------------------------------------------------------------

/*
	ズーム値からズーム倍率を計算して返す。

	ズーム値は px 単位で、マイナスになることもあるが、
	ズーム倍率は 0.0 より大きい数値になる。
*/
double MainWindow::getZoomScale()
{
	return std::pow(2.0, (double)m_zoom / g_design.scale.horz.minUnitWidth);
}

BOOL MainWindow::getLayoutContext(LayoutContext& context)
{
	RECT rc; ::GetClientRect(m_hwnd, &rc);
	return getLayoutContext(context, rc);
}

BOOL MainWindow::getLayoutContext(LayoutContext& context, const RECT& rc)
{
	context.rc = rc;
	context.x = rc.left;
	context.y = rc.top;
	context.w = getWidth(rc);
	context.h = getHeight(rc);

	if (context.w <= 0 || context.h <= 0) return FALSE;

	context.hScroll = ::GetScrollPos(m_hwnd, SB_HORZ);

	switch (m_vertScaleSettings.visibleStyle)
	{
	case VertScaleSettings::VisibleStyle::left:
		{
			context.graph.x = (int)(context.x + g_design.body.margin.x);
			context.graph.w = (int)(context.w - g_design.body.margin.x);
			break;
		}
	case VertScaleSettings::VisibleStyle::right:
		{
			context.graph.x = (int)(context.x);
			context.graph.w = (int)(context.w - g_design.body.margin.x);
			break;
		}
	case VertScaleSettings::VisibleStyle::both:
		{
			context.graph.x = (int)(context.x + g_design.body.margin.x);
			context.graph.w = (int)(context.w - g_design.body.margin.x * 2);
			break;
		}
	}

	switch (m_horzScaleSettings.visibleStyle)
	{
	case HorzScaleSettings::VisibleStyle::top:
		{
			context.graph.y = (int)(context.y + g_design.body.margin.y);
			context.graph.h = (int)(context.h - g_design.body.margin.y);
			break;
		}
	case HorzScaleSettings::VisibleStyle::bottom:
		{
			context.graph.y = (int)(context.y);
			context.graph.h = (int)(context.h - g_design.body.margin.y);
			break;
		}
	case HorzScaleSettings::VisibleStyle::both:
		{
			context.graph.y = (int)(context.y + g_design.body.margin.y);
			context.graph.h = (int)(context.h - g_design.body.margin.y * 2);
			break;
		}
	}

	context.graph.left = context.graph.x;
	context.graph.right = context.graph.x + context.graph.w;
	context.graph.top = context.graph.y;
	context.graph.bottom = context.graph.y + context.graph.h;
	context.padding = 10;

	if (context.graph.w <= 0 || context.graph.h <= 0) return FALSE;

	context.zoomScale = getZoomScale();

	return TRUE;
}

//--------------------------------------------------------------------

void MainWindow::recalcLayout()
{
	MY_TRACE(_T("MainWindow::recalcLayout()\n"));

	int c = (int)theApp.totals.size();
	if (c <= 1) return;

	LayoutContext context = {};
	if (!getLayoutContext(context)) return;

	double sec = frame2sec(c - 1);
	int logicalWidth = sec2px(context.zoomScale, sec);

	SCROLLINFO si = { sizeof(si) };
	si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	::GetScrollInfo(m_hwnd, SB_HORZ, &si);

	double pos = 0.0;
	int div = si.nMax - std::max((int)si.nPage - 1, 0);
	if (div)
		pos = (double)si.nPos / div;

	si.nPos = (int)(pos * (logicalWidth - context.graph.w));
	si.nMin = 0;
	si.nMax = logicalWidth;
	si.nPage = context.graph.w + 1;
	::SetScrollInfo(m_hwnd, SB_HORZ, &si, TRUE);
}

int MainWindow::hitTest(POINT point)
{
	RECT rc; ::GetClientRect(*this, &rc);
	int cy = getCenterY(rc);

	if (m_horzScaleSettings.visibleStyle != HorzScaleSettings::VisibleStyle::bottom)
	{
		if (point.y < rc.top + g_design.body.margin.y)
			return HitTest::HorzScale;
	}

	if (m_horzScaleSettings.visibleStyle != HorzScaleSettings::VisibleStyle::top)
	{
		if (point.y > rc.bottom - g_design.body.margin.y)
			return HitTest::HorzScale;
	}

	if (m_vertScaleSettings.visibleStyle != VertScaleSettings::VisibleStyle::right)
	{
		if (point.x < rc.left + g_design.body.margin.x)
			return (point.y > cy) ? HitTest::VertScaleMin : HitTest::VertScaleMax;
	}

	if (m_vertScaleSettings.visibleStyle != VertScaleSettings::VisibleStyle::left)
	{
		if (point.x > rc.right - g_design.body.margin.x)
			return (point.y > cy) ? HitTest::VertScaleMin : HitTest::VertScaleMax;
	}

	return HitTest::None;
}

int MainWindow::client2frame(int x)
{
	LayoutContext context = {};
	if (!getLayoutContext(context)) return 0;

	return client2frame(context, x);
}

/*
	クライアント座標の x からフレーム番号を取得する。
	・グラフの X 座標が影響する。
	・水平スクロールが影響する。
*/
int MainWindow::client2frame(const LayoutContext& context, int x)
{
	double sec = px2sec(context.zoomScale, x + context.hScroll - context.graph.x);
	return sec2frame(sec);
}

int MainWindow::frame2client(int frame)
{
	LayoutContext context = {};
	if (!getLayoutContext(context)) return 0;

	return frame2client(context, frame);
}

/*
	フレーム番号からクライアント座標を取得する。
	・グラフの X 座標が影響する。
	・水平スクロールが影響する。
*/
int MainWindow::frame2client(const LayoutContext& context, int frame)
{
	double sec = frame2sec(frame);
	return sec2px(context.zoomScale, sec) - context.hScroll + context.graph.x;
}

int MainWindow::sec2frame(double sec)
{
	if (!theApp.projectParams) return 0;

	int video_rate = theApp.projectParams->video_rate;
	int video_scale = theApp.projectParams->video_scale;

	if (video_rate == 0 || video_scale == 0)
		return 0;

	return (int)(sec * video_rate / video_scale);
}

double MainWindow::frame2sec(int frame)
{
	if (!theApp.projectParams) return 0;

	int video_rate = theApp.projectParams->video_rate;
	int video_scale = theApp.projectParams->video_scale;

	if (video_rate == 0 || video_scale == 0)
		return 0;

	return (double)frame * video_scale / video_rate;
}

int MainWindow::sec2px(double zoomScale, double sec)
{
	return (int)(sec * g_design.scale.horz.minUnitWidth * zoomScale);
}

double MainWindow::px2sec(double zoomScale, int px)
{
	return (double)px / g_design.scale.horz.minUnitWidth / zoomScale;
}

double MainWindow::getUnitSec(double zoomScale)
{
	double secMinWidth = px2sec(zoomScale, g_design.scale.horz.minUnitWidth);
	double sec = 1.0;

	if (zoomScale > 1.0)
	{
		// 単位秒は 1 より小さい。

		while (1)
		{
			const double list[] =
			{
				2.0,
				3.0,
				4.0,
				5.0,
				10.0,
			};

			double prev = sec;

			for (int i = 0; i < _countof(list); i++)
			{
				double temp = sec / list[i];

				if (temp < secMinWidth)
					return prev;

				prev = temp;
			}

			sec /= 10.0;
		}
	}
	else
	{
		// 単位秒は 1 より大きい。

		while (1)
		{
			const double list[] =
			{
				1.0,
				2.0,
				3.0,
				4.0,
				5.0,
				6.0,
			};

			for (int i = 0; i < _countof(list); i++)
			{
				double temp = sec * list[i];

				if (temp >= secMinWidth)
					return temp;
			}

			sec *= 10.0;
		}
	}

	return sec;
}

//--------------------------------------------------------------------

void MainWindow::outputFrames()
{
	MY_TRACE(_T("outputFrames()\n"));

	if (!theApp.projectParams) return;

	TCHAR text[1024] = {};

	int c = (int)theApp.totals.size();
	int currentFrame = theApp.projectParams->currentFrame;
	int hotFrame = getHotFrame();

	if (currentFrame >= 0 && currentFrame < c)
	{
		FormatText subText(_T("現在位置 : %.2fdB @%d\r\n"), theApp.totals[currentFrame].rms, currentFrame);
		::StringCbCat(text, sizeof(text), subText);
	}

	if (hotFrame >= 0 && hotFrame < c)
	{
		FormatText subText(_T("マウス位置 : %.2fdB @%d\r\n"), theApp.totals[hotFrame].rms, hotFrame);
		::StringCbCat(text, sizeof(text), subText);
	}

	::SetDlgItemText(theApp.mainDialog, IDC_STATUS, text);
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
//	int thumbPos = HIWORD(wParam);
	HWND scrollBar = (HWND)lParam;

	SCROLLINFO si = { sizeof(si) };
	si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE | SIF_TRACKPOS;
	::GetScrollInfo(hwnd, SB_HORZ, &si);

	MY_TRACE(_T("MainWindow::onHScroll(%d, %d, 0x%08X)\n"), sb, si.nTrackPos, scrollBar);

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
	case SB_THUMBTRACK: pos = si.nTrackPos; break;
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

//--------------------------------------------------------------------
