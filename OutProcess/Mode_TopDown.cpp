#include "pch.h"
#include "Mode.h"
#include "MainWindow.h"
#include "App.h"

//--------------------------------------------------------------------

int TopDownMode::getID() const
{
	return MainWindow::Mode::topDown;
}

//--------------------------------------------------------------------

void TopDownMode::drawVertScale(MainWindow& window, const LayoutContext& context)
{
	// 垂直目盛りを左右に描画する。

	if (window.m_limitVolume <= 0) return;

	NVGcontext* vg = window.m_vg;
	const LayoutContext::Graph& graph = context.graph;

	nvgScissor(vg, (float)context.x, (float)graph.y, (float)context.w, (float)graph.h);

	int textPadding = 4;
	int textHeight = g_design.scale.vert.text.height + textPadding * 2;

	int prev = -(textHeight + 1);
	for (int i = 0; i <= 10; i++)
	{
		int oy = graph.h * i / 10;
		if (oy - prev < textHeight) continue;
		prev = oy;

		float fy = (float)(graph.top + oy);

		drawVertScaleLine(window, context, fy);

		char text[MAX_PATH] = {};
		::StringCbPrintfA(text, sizeof(text), "%d", window.m_limitVolume * i / 10);

		{
			nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM);

			float tx = (float)(graph.left - textPadding);
			float ty = (float)(fy - textPadding);

			drawText(vg, text, tx, ty, g_design.scale.vert.text);
		}

		{
			nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);

			float tx = (float)(graph.right + textPadding);
			float ty = (float)(fy - textPadding);

			drawText(vg, text, tx, ty, g_design.scale.vert.text);
		}
	}

	{
		// 基準音量を描画する。

		int y = graph.top + graph.h * window.m_baseVolume / window.m_limitVolume;

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

void TopDownMode::drawGraph(MainWindow& window, const LayoutContext& context)
{
	int c = (int)theApp.fullSamples.size();

	if (c <= 0) return;
	if (window.m_limitVolume <= 0) return;

	NVGcontext* vg = window.m_vg;
	const LayoutContext::Graph& graph = context.graph;

	nvgScissor(vg, (float)graph.x, (float)graph.y, (float)graph.w, (float)graph.h);

	int ox = graph.x - context.hScroll; // 描画範囲の X 座標。
	int lh = graph.h * 100 / window.m_limitVolume;

	// グラフを描画する。

	std::vector<PointF> points;

	{
		// まず、描画に必要な座標をすべて取得する。

		for (int i = 0; i < c; i++)
		{
			float level = theApp.fullSamples[i].level;
			float x = (float)window.frame2client(i);
			float y = (float)graph.top + lh * level;

			points.emplace_back(x, y);
		}
	}

	{
		// グラフを塗りつぶす。

		nvgBeginPath(vg);
		nvgMoveTo(vg, (float)graph.left, (float)(graph.top - context.padding));

		for (const auto& point : points)
			nvgLineTo(vg, point.x, point.y);

		nvgLineTo(vg, (float)graph.right, (float)(graph.top - context.padding));

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
		nvgMoveTo(vg, (float)graph.left, (float)graph.top);

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
