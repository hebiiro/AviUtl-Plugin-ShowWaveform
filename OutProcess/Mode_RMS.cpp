#include "pch.h"
#include "Mode.h"
#include "MainWindow.h"
#include "App.h"

//--------------------------------------------------------------------

int RMSMode::getID() const
{
	return MainWindow::Mode::rms;
}

//--------------------------------------------------------------------

void RMSMode::drawVertScale(MainWindow& window, const LayoutContext& context)
{
	// 垂直目盛りを左右に描画する。

	NVGcontext* vg = window.m_vg;
	const LayoutContext::Graph& graph = context.graph;

	nvgScissor(vg, (float)context.x, (float)graph.y, (float)context.w, (float)graph.h);

	int textPadding = 4;
	int textHeight = g_design.scale.vert.text.height + textPadding * 2;

	int minRMS = window.getMinRMS();
	int maxRMS = window.getMaxRMS();
	int baseRMS = window.getBaseRMS();

	float range = (float)(maxRMS - minRMS);
	MY_TRACE_REAL(range);
	int freq = (int)(range * textHeight / graph.h) + 1;
	MY_TRACE_INT(freq);

	for (int i = maxRMS; i > minRMS - freq; i--)
	{
		if (i % freq) continue;

		float relativeLevel = (float)(i - minRMS);
		int y = (int)(graph.y + graph.h * (1.0f - relativeLevel / range));
		float fy = (float)y;

		drawVertScaleLine(window, context, fy);

		char text[MAX_PATH] = {};
		::StringCbPrintfA(text, sizeof(text), "%+d", i);

		{
			nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM);

			float tx = (float)(graph.left - textPadding);
			float ty = (float)(y - textPadding);

			drawText(vg, text, tx, ty, g_design.scale.vert.text);
		}

		{
			nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);

			float tx = (float)(graph.right + textPadding);
			float ty = (float)(y - textPadding);

			drawText(vg, text, tx, ty, g_design.scale.vert.text);
		}
	}

	{
		// 基準 RMS を描画する。

		float relativeLevel = (float)(baseRMS - minRMS);
		int y = (int)(graph.top + graph.h * (1.0f - relativeLevel / range));

		if (y > graph.top && y < graph.bottom)
		{
			float fy = (float)y;
			float mx = (float)context.rc.left;
			float lx = (float)context.rc.right;

			drawLine(vg, mx, fy, lx, fy, g_design.scale.vert.base.stroke);
		}
	}

	nvgResetScissor(vg);
}

void RMSMode::drawGraph(MainWindow& window, const LayoutContext& context)
{
	int c = (int)theApp.totals.size();

	if (c <= 0) return;

	NVGcontext* vg = window.m_vg;
	const LayoutContext::Graph& graph = context.graph;

	nvgScissor(vg, (float)graph.x, (float)graph.y, (float)graph.w, (float)graph.h);

	int ox = graph.x - context.hScroll; // 描画範囲の X 座標。
	int minRMS = window.getMinRMS();
	int maxRMS = window.getMaxRMS();

	// グラフを描画する。

	std::vector<PointF> points;

	{
		// まず、描画に必要な座標をすべて取得する。

		for (int i = 0; i < c; i++)
		{
			float level = (theApp.totals[i].rms - minRMS) / (maxRMS - minRMS);
			float x = (float)window.frame2client(i);
			float y = std::min((float)graph.bottom, (float)graph.bottom - graph.h * level);

			points.emplace_back(x, y);
		}
	}

	{
		// グラフを塗りつぶす。

		nvgBeginPath(vg);
		nvgMoveTo(vg, (float)graph.left, (float)(graph.bottom + context.padding));

		for (const auto& point : points)
			nvgLineTo(vg, point.x, point.y);

		nvgLineTo(vg, (float)graph.right, (float)(graph.bottom + context.padding));
//		nvgClosePath(vg);
//		nvgPathWinding(vg, NVG_SOLID);

		NVGpaint paint = nvgLinearGradient(vg,
			(float)graph.left, (float)graph.top,
			(float)graph.left, (float)graph.bottom,
			g_design.graph.fill.color1, g_design.graph.fill.color2);
		nvgFillPaint(vg, paint);
		nvgFill(vg);
	}

	{
		// グラフのストロークを描画する。

		nvgBeginPath(vg);
		nvgMoveTo(vg, (float)graph.left, (float)graph.bottom);

		for (const auto& point : points)
			nvgLineTo(vg, point.x, point.y);

		nvgStrokeWidth(vg, (float)g_design.graph.stroke.width);
		nvgStrokeColor(vg, g_design.graph.stroke.color);
		nvgStroke(vg);
	}

	nvgResetScissor(vg);

	Mode::drawGraph(window, context);
}

//--------------------------------------------------------------------
