#include "pch.h"
#include "Mode.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

int RMSMode::getID() const
{
	return MainWindow::Mode::rms;
}

//--------------------------------------------------------------------

void RMSMode::drawScale(MainWindow& window, const PaintContext& context)
{
	// ゲージを描画する。

	NVGcontext* vg = window.m_vg;

	int textPadding = 1;
	int textHeight = g_design.scale.text.height + textPadding * 2;

	int range = window.m_maxRange - window.m_minRange;
	int freq = range * textHeight / context.height + 1;

	{
		// ベースレベルを描画する。

		float relativeLevel = (float)(window.m_baseLevel - window.m_minRange);
		float y = context.rc.top + context.height * (1.0f - relativeLevel / range);

		float mx = (float)(context.rc.left + g_design.body.margin);
		float my = (float)(y);
		float lx = (float)(context.rc.right - g_design.body.margin);
		float ly = (float)(y);

		nvgStrokeWidth(vg, (float)g_design.scale.base.stroke.width);
		nvgStrokeColor(vg, g_design.scale.base.stroke.color);

		nvgBeginPath(vg);
		nvgMoveTo(vg, mx, my);
		nvgLineTo(vg, lx, ly);
		nvgStroke(vg);
	}

	nvgStrokeWidth(vg, (float)g_design.scale.stroke.width);
	nvgStrokeColor(vg, g_design.scale.stroke.color);
	nvgFontSize(vg, (float)g_design.scale.text.height);
	nvgFontFaceId(vg, window.m_fontDefault);

	for (int i = window.m_maxRange; i > window.m_minRange - freq; i--)
	{
		if (i % freq) continue;

		float relativeLevel = (float)(i - window.m_minRange);
		float y = context.rc.top + context.height * (1.0f - relativeLevel / range);

		{
			float mx = (float)(context.rc.left + g_design.body.margin);
			float my = (float)(y);
			float lx = (float)(context.rc.left + g_design.body.margin - g_design.scale.width);
			float ly = (float)(y);

			nvgBeginPath(vg);
			nvgMoveTo(vg, mx, my);
			nvgLineTo(vg, lx, ly);
			nvgStroke(vg);
		}

		{
			float mx = (float)(context.rc.right - g_design.body.margin);
			float my = (float)(y);
			float lx = (float)(context.rc.right - g_design.body.margin + g_design.scale.width);
			float ly = (float)(y);

			nvgBeginPath(vg);
			nvgMoveTo(vg, mx, my);
			nvgLineTo(vg, lx, ly);
			nvgStroke(vg);
		}

		char text[MAX_PATH] = {};
		::StringCbPrintfA(text, sizeof(text), "%+d", i);

		{
			nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM);

			float tx = (float)(context.rc.left + g_design.body.margin - textPadding);
			float ty = y - textPadding;

			drawText(window, context, tx, ty, text, g_design.scale.text);
		}

		{
			nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);

			float tx = (float)(context.rc.right - g_design.body.margin + textPadding);
			float ty = y - textPadding;

			drawText(window, context, tx, ty, text, g_design.scale.text);
		}
	}
}

void RMSMode::drawGraph(MainWindow& window, const PaintContext& context)
{
	NVGcontext* vg = window.m_vg;

	int c = (int)window.fullSamples.size();

	if (c <= 0) return;

	// グラフを描画する。

	struct PointF { float x, y; };
	std::vector<PointF> points;
	for (int i = 0; i < c; i++)
	{
		float level = (window.fullSamples[i].rms - window.m_minRange) / (window.m_maxRange - window.m_minRange);
		float x = context.left + context.lgw * i / (c - 1) - context.hScroll;
		float y = std::min(context.bottom, context.bottom - context.gh * level);

		if (x >= context.left && x <= context.right)
			points.emplace_back(x, y);
	}

	{
		// グラフを塗りつぶす。

		nvgBeginPath(vg);
		nvgMoveTo(vg, context.left, context.bottom + context.padding);

		for (const auto& point : points)
			nvgLineTo(vg, point.x, point.y);

		nvgLineTo(vg, context.right, context.bottom + context.padding);
//		nvgClosePath(vg);
//		nvgPathWinding(vg, NVG_SOLID);

		NVGpaint paint = nvgLinearGradient(vg,
			context.x, context.y, context.x, context.y + context.h,
			g_design.graph.fill.color1, g_design.graph.fill.color2);
		nvgFillPaint(vg, paint);
		nvgFill(vg);
	}

	{
		// グラフのストロークを描画する。

		nvgBeginPath(vg);
		nvgMoveTo(vg, context.left, context.bottom);

		for (const auto& point : points)
			nvgLineTo(vg, point.x, point.y);

		nvgStrokeWidth(vg, (float)g_design.graph.stroke.width);
		nvgStrokeColor(vg, g_design.graph.stroke.color);
		nvgStroke(vg);
	}

	drawMarkers(window, context);
}

//--------------------------------------------------------------------

LRESULT RMSMode::onMouseMove(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	MY_TRACE(_T("RMSMode::onMouseMove(0x%08X, 0x%08X)\n"), wParam, lParam);

	POINT point = LP2PT(lParam);

	if (::GetCapture() == hwnd)
	{
		// minRange または maxRange をドラッグして変更する。

		int offset = point.y - m_dragOriginPoint.y;

		switch (m_dragMode)
		{
		case DragMode::maxRange: window.m_maxRange = m_dragOriginRange + offset / 5; break;
		case DragMode::minRange: window.m_minRange = m_dragOriginRange + offset / 5; break;
		}

		::InvalidateRect(hwnd, 0, FALSE);
	}

	Mode::onMouseMove(window, point);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT RMSMode::onLButtonDown(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("RMSMode::onLButtonDown(0x%08X, 0x%08X)\n"), wParam, lParam);

	POINT point = LP2PT(lParam);
	RECT rc; ::GetClientRect(hwnd, &rc);

	if (point.y < rc.top + g_design.body.margin ||
		point.y > rc.bottom - g_design.body.margin)
	{
		// minRange または maxRange のドラッグを開始する。

		::SetCapture(hwnd);
		m_dragOriginPoint = point;

		RECT rc; ::GetClientRect(hwnd, &rc);
		int cy = (rc.top + rc.bottom) / 2;

		if (m_dragOriginPoint.y < cy)
		{
			m_dragMode = DragMode::maxRange;
			m_dragOriginRange = window.m_maxRange;
		}
		else
		{
			m_dragMode = DragMode::minRange;
			m_dragOriginRange = window.m_minRange;
		}
	}
	else
	{
		Mode::onLButtonDown(window, point);
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT RMSMode::onLButtonUp(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("RMSMode::onLButtonUp(0x%08X, 0x%08X)\n"), wParam, lParam);

	::ReleaseCapture();

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------
