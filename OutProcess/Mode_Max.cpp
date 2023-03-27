#include "pch.h"
#include "Mode.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

int MaxMode::getID() const
{
	return MainWindow::Mode::max;
}

//--------------------------------------------------------------------

void MaxMode::drawScale(MainWindow& window, const PaintContext& context)
{
	// ゲージを描画する。

	NVGcontext* vg = window.m_vg;

	float lgh = (float)(context.height - g_design.body.margin) * (100 + window.m_scale) / 100.0f;

	int textPadding = 1;
	int textHeight = g_design.scale.text.height + textPadding * 2;

	nvgStrokeWidth(vg, (float)g_design.scale.stroke.width);
	nvgStrokeColor(vg, g_design.scale.stroke.color);
	nvgFontSize(vg, (float)g_design.scale.text.height);
	nvgFontFaceId(vg, window.m_fontDefault);

	int prev = -(textHeight + 1);
	for (int i = 0; i <= 10; i++)
	{
		int oy = (int)(lgh * i / 10);
		if (oy - prev < textHeight) continue;
		prev = oy;

		float y = context.top + oy;

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
		::StringCbPrintfA(text, sizeof(text), "%.1f", i / 10.0);

		{
			nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);

			float tx = (float)(context.rc.left + g_design.body.margin - textPadding);
			float ty = (float)(y + textPadding + 1);

			drawText(window, context, tx, ty, text, g_design.scale.text);
		}

		{
			nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);

			float tx = (float)(context.rc.right - g_design.body.margin + textPadding);
			float ty = (float)(y + textPadding + 1);

			drawText(window, context, tx, ty, text, g_design.scale.text);
		}
	}
}

void MaxMode::drawGraph(MainWindow& window, const PaintContext& context)
{
	NVGcontext* vg = window.m_vg;

	int c = (int)window.fullSamples.size();

	if (c <= 0) return;

	float lgh = (float)(context.height - g_design.body.margin) * (100 + window.m_scale) / 100.0f;
	MY_TRACE_INT(window.m_scale);
	MY_TRACE_REAL(lgh);

	// グラフを描画する。

	struct PointF { float x, y; };
	std::vector<PointF> points;
	for (int i = 0; i < c; i++)
	{
		float level = window.fullSamples[i].level;
		float x = context.left + context.lgw * i / (c - 1) - context.hScroll;
		float y = context.top + lgh * level;

		if (x >= context.left && x <= context.right)
			points.emplace_back(x, y);
	}

	{
		// グラフを塗りつぶす。

		nvgBeginPath(vg);
		nvgMoveTo(vg, context.left, context.top - context.padding);

		for (const auto& point : points)
			nvgLineTo(vg, point.x, point.y);

		nvgLineTo(vg, context.right, context.top - context.padding);

		NVGpaint paint = nvgLinearGradient(vg,
			context.x, context.y, context.x, context.y + context.h,
			g_design.graph.fill.color1, g_design.graph.fill.color2);
		nvgFillPaint(vg, paint);
		nvgFill(vg);
	}

	{
		// グラフのストロークを描画する。

		nvgBeginPath(vg);
		nvgMoveTo(vg, context.left, context.top);

		for (const auto& point : points)
			nvgLineTo(vg, point.x, point.y);

		nvgStrokeWidth(vg, (float)g_design.graph.stroke.width);
		nvgStrokeColor(vg, g_design.graph.stroke.color);
		nvgStroke(vg);
	}

	drawMarkers(window, context);
}

//--------------------------------------------------------------------

LRESULT MaxMode::onMouseMove(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	MY_TRACE(_T("MaxMode::onMouseMove(0x%08X, 0x%08X)\n"), wParam, lParam);

	POINT point = LP2PT(lParam);

	if (::GetCapture() == hwnd)
	{
		// scale をドラッグして変更する。

		int offset = point.y - m_dragOriginPoint.y;

		window.m_scale = m_dragOriginScale + offset;

		::InvalidateRect(hwnd, 0, FALSE);
	}

	Mode::onMouseMove(window, point);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------
