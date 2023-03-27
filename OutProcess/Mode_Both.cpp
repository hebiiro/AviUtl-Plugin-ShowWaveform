﻿#include "pch.h"
#include "Mode.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

int BothMode::getID() const
{
	return MainWindow::Mode::both;
}

//--------------------------------------------------------------------

void BothMode::drawScale(MainWindow& window, const PaintContext& context)
{
	// ゲージを描画する。

	NVGcontext* vg = window.m_vg;

	float cy = context.gy + context.gh / 2.0f;
	float lgh = (float)(context.height - g_design.body.margin) * (100 + window.m_scale) / 100.0f / 2.0f;

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

		float y[2] = { cy - oy, cy + oy };
		int c = (y[0] == y[1]) ? 1 : 2;

		for (int j = 0; j < c; j++)
		{
			{
				float mx = (float)(context.rc.left + g_design.body.margin);
				float my = (float)(y[j]);
				float lx = (float)(context.rc.left + g_design.body.margin - g_design.scale.width);
				float ly = (float)(y[j]);

				nvgBeginPath(vg);
				nvgMoveTo(vg, mx, my);
				nvgLineTo(vg, lx, ly);
				nvgStroke(vg);
			}

			{
				float mx = (float)(context.rc.right - g_design.body.margin);
				float my = (float)(y[j]);
				float lx = (float)(context.rc.right - g_design.body.margin + g_design.scale.width);
				float ly = (float)(y[j]);

				nvgBeginPath(vg);
				nvgMoveTo(vg, mx, my);
				nvgLineTo(vg, lx, ly);
				nvgStroke(vg);
			}

			char text[MAX_PATH] = {};
			::StringCbPrintfA(text, sizeof(text), "%.1f", i / 10.0);
			MY_TRACE_STR(text);

			{
				nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM);

				float tx = (float)(context.rc.left + g_design.body.margin - textPadding);
				float ty = (float)(y[j] - textPadding);

				drawText(window, context, tx, ty, text, g_design.scale.text);
			}

			{
				nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);

				float tx = (float)(context.rc.right - g_design.body.margin + textPadding);
				float ty = (float)(y[j] - textPadding);

				drawText(window, context, tx, ty, text, g_design.scale.text);
			}
		}
	}
}

void BothMode::drawGraph(MainWindow& window, const PaintContext& context)
{
	NVGcontext* vg = window.m_vg;

	int c = (int)window.fullSamples.size();

	if (c <= 0) return;

	float cy = context.gy + context.gh / 2.0f;
	float lgh = (float)(context.height - g_design.body.margin) * (100 + window.m_scale) / 100.0f / 2.0f;
	MY_TRACE_INT(window.m_scale);
	MY_TRACE_REAL(lgh);

	// グラフを描画する。

	struct PointF { float x, y; };
	std::vector<PointF> points;
	for (int i = 0; i < c; i++)
	{
		float level = window.fullSamples[i].level;
		float x = context.left + context.lgw * i / (c - 1) - context.hScroll;
		float y = lgh * level;

		if (x >= context.left && x <= context.right)
			points.emplace_back(x, y);
	}

	{
		// グラフを塗りつぶす。

		nvgBeginPath(vg);
		nvgMoveTo(vg, context.left, cy);

		for (const auto& point : points)
			nvgLineTo(vg, point.x, cy - point.y);

		int c = (int)points.size();
		for (int i = c - 1; i >= 0; i--)
			nvgLineTo(vg, points[i].x, cy + points[i].y);

		NVGpaint paint = nvgLinearGradient(vg,
			context.x, context.y, context.x, context.y + context.h,
			g_design.graph.fill.color1, g_design.graph.fill.color2);
		nvgFillPaint(vg, paint);
		nvgFill(vg);
	}

	{
		// グラフのストロークを描画する。

		nvgBeginPath(vg);
		nvgMoveTo(vg, context.left, cy);

		for (const auto& point : points)
			nvgLineTo(vg, point.x, cy - point.y);

		int c = (int)points.size();
		for (int i = c - 1; i >= 0; i--)
			nvgLineTo(vg, points[i].x, cy + points[i].y);

		nvgStrokeWidth(vg, (float)g_design.graph.stroke.width);
		nvgStrokeColor(vg, g_design.graph.stroke.color);
		nvgStroke(vg);
	}

	drawMarkers(window, context);
}

//--------------------------------------------------------------------
