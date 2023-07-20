#include "pch.h"
#include "Mode.h"
#include "MainWindow.h"
#include "App.h"

//--------------------------------------------------------------------

int CenterMode::getID() const
{
	return MainWindow::Mode::center;
}

//--------------------------------------------------------------------

void CenterMode::drawVertScale(MainWindow& window, const LayoutContext& context)
{
	// 垂直目盛りを左右に描画する。

	if (window.m_limitVolume <= 0) return;

	NVGcontext* vg = window.m_vg;
	const LayoutContext::Graph& graph = context.graph;

	nvgScissor(vg, (float)context.x, (float)graph.y, (float)context.w, (float)graph.h);

	int textPadding = 4;
	int textHeight = g_design.scale.vert.text.height + textPadding * 2;
	int cy = graph.y + graph.h / 2;

	int prev = -(textHeight + 1);
	for (int i = 0; i <= 10; i++)
	{
		int oy = graph.h * i / 10;
		if (oy - prev < textHeight) continue;
		prev = oy;

		int y[2] = { cy - oy, cy + oy };
		int c = (y[0] == y[1]) ? 1 : 2;

		for (int j = 0; j < c; j++)
		{
			drawVertScaleLine(window, context, (float)y[j]);

			char text[MAX_PATH] = {};
			::StringCbPrintfA(text, sizeof(text), "%d", window.m_limitVolume * i / 10);

			{
				nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM);

				float tx = (float)(graph.left - textPadding);
				float ty = (float)(y[j] - textPadding);

				drawText(vg, text, tx, ty, g_design.scale.vert.text);
			}

			{
				nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);

				float tx = (float)(graph.right + textPadding);
				float ty = (float)(y[j] - textPadding);

				drawText(vg, text, tx, ty, g_design.scale.vert.text);
			}
		}
	}

	{
		// 基準音量を描画する。

		int oy = graph.h * window.m_baseVolume / window.m_limitVolume;

		for (int j = 0; j < 2; j++)
		{
			int y[2] = { cy - oy, cy + oy };

			if (y[j] > graph.top && y[j] < graph.bottom)
			{
				float fy = (float)y[j];
				float mx = (float)context.rc.left;
				float lx = (float)context.rc.right;

				drawLine(vg, mx, fy, lx, fy, g_design.scale.vert.base.stroke);
			}
		}
	}

	nvgResetScissor(vg);
}

void CenterMode::drawGraph(MainWindow& window, const LayoutContext& context)
{
	int c = (int)theApp.totals.size();

	if (c <= 0) return;
	if (window.m_limitVolume <= 0) return;

	NVGcontext* vg = window.m_vg;
	const LayoutContext::Graph& graph = context.graph;

	nvgScissor(vg, (float)graph.x, (float)graph.y, (float)graph.w, (float)graph.h);

	int ox = graph.x - context.hScroll; // 描画範囲の X 座標。
	int lh = graph.h * 100 / window.m_limitVolume;
	int cy = graph.y + graph.h / 2;

	// グラフを描画する。

	std::vector<PointF> points;

	{
		// まず、描画に必要な座標をすべて取得する。

		for (int i = 0; i < c; i++)
		{
			float level = theApp.totals[i].level;
			float x = (float)window.frame2client(i);
			float y = (float)lh * level;

			points.emplace_back(x, y);
		}
	}

	{
		// グラフを塗りつぶす。

		nvgBeginPath(vg);
		nvgMoveTo(vg, (float)graph.left, (float)cy);

		for (const auto& point : points)
			nvgLineTo(vg, point.x, cy - point.y);

		int c = (int)points.size();
		for (int i = c - 1; i >= 0; i--)
			nvgLineTo(vg, points[i].x, cy + points[i].y);

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
		nvgMoveTo(vg, (float)graph.left, (float)cy);

		for (const auto& point : points)
			nvgLineTo(vg, point.x, cy - point.y);

		int c = (int)points.size();
		for (int i = c - 1; i >= 0; i--)
			nvgLineTo(vg, points[i].x, cy + points[i].y);

		nvgStrokeWidth(vg, (float)g_design.graph.stroke.width);
		nvgStrokeColor(vg, g_design.graph.stroke.color);
		nvgStroke(vg);
	}

	nvgResetScissor(vg);

	Mode::drawGraph(window, context);
}

//--------------------------------------------------------------------
