#include "pch.h"
#include "Mode.h"
#include "MainWindow.h"
#include "App.h"

//--------------------------------------------------------------------

NVGpaint Mode::nvgVertGradient(NVGcontext* vg, const XYWHRect& rc, const Design::GradientFill& design)
{
	return nvgLinearGradient(vg,
		(float)rc.x, (float)rc.y, (float)rc.x, (float)(rc.y + rc.h),
		design.color1, design.color2);
};

void Mode::nvgStrokeDesign(NVGcontext* vg, const Design::Stroke& stroke)
{
	nvgLineStyle(vg, stroke.style);
	nvgStrokeWidth(vg, (float)stroke.width);
	nvgStrokeColor(vg, stroke.color);
}

void Mode::nvgXYWHRect(NVGcontext* vg, const XYWHRect& rc)
{
	nvgRect(vg, (float)rc.x, (float)rc.y, (float)rc.w, (float)rc.h);
}

void Mode::drawImage(NVGcontext* vg, int image, const XYWHRect& rc, const Design::Image& design)
{
	if (!image) return;

	int imgw = 0, imgh = 0;
	nvgImageSize(vg, image, &imgw, &imgh);

	XYWHRect dst, src;

	dst.x = src.x = 0;
	dst.y = src.y = 0;
	dst.w = src.w = imgw;
	dst.h = src.h = imgh;

	switch (design.scaleMode)
	{
	case Design::ScaleMode::fit:
		{
			dst.w = rc.w;
			dst.h = rc.w * imgh / imgw;

			if (dst.h > rc.h)
			{
				dst.w = rc.h * imgw / imgh;
				dst.h = rc.h;
			}

			break;
		}
	case Design::ScaleMode::crop:
		{
			dst.w = rc.w;
			dst.h = rc.w * imgh / imgw;

			if (dst.h < rc.h)
			{
				dst.w = rc.h * imgw / imgh;
				dst.h = rc.h;
			}

			break;
		}
	}

	int align = 0;

	switch (design.align.x)
	{
	case Design::AlignX::left: align |= NVG_ALIGN_LEFT; break;
	case Design::AlignX::right: align |= NVG_ALIGN_RIGHT; break;
	case Design::AlignX::center: align |= NVG_ALIGN_CENTER; break;
	}

	switch (design.align.y)
	{
	case Design::AlignY::top: align |= NVG_ALIGN_TOP; break;
	case Design::AlignY::bottom: align |= NVG_ALIGN_BOTTOM; break;
	case Design::AlignY::center: align |= NVG_ALIGN_MIDDLE; break;
	}

	if (align & NVG_ALIGN_RIGHT)
	{
		dst.x = (rc.x + rc.w) - dst.w;
		dst.x -= design.offset.x;
	}
	else
	{
		if (align & NVG_ALIGN_CENTER)
			dst.x = rc.x + (rc.w - dst.w) / 2;
	
		dst.x += design.offset.x;
	}

	if (align & NVG_ALIGN_BOTTOM)
	{
		dst.y = (rc.y + rc.h) - dst.h;
		dst.y -= design.offset.y;
	}
	else
	{
		if (align & NVG_ALIGN_MIDDLE)
			dst.y = rc.y + (rc.h - dst.h) / 2;
			
		dst.y += design.offset.y;
	}

	NVGpaint imgPaint = nvgImagePattern(vg,
		(float)dst.x, (float)dst.y, (float)dst.w, (float)dst.h,
		design.angle / 360.0f * (2.0f * NVG_PI), image, design.alpha);
	nvgBeginPath(vg);
	nvgXYWHRect(vg, rc);
	nvgFillPaint(vg, imgPaint);
	nvgFill(vg);
}

void Mode::drawLine(NVGcontext* vg, float mx, float my, float lx, float ly, const Design::Stroke& stroke)
{
	const float offset = -0.5f;

	nvgBeginPath(vg);
	nvgMoveTo(vg, mx + offset, my + offset);
	nvgLineTo(vg, lx + offset, ly + offset);
	nvgStrokeDesign(vg, stroke);
	nvgStroke(vg);
}

void Mode::drawText(NVGcontext* vg, LPCSTR text, float x, float y, const Design::Text& design)
{
	float xText = x;
	float yText = y;

	nvgFontSize(vg, (float)design.height);

	if (design.shadow.dilate > 0.0f || design.shadow.blur > 0.0f)
	{
		float xTextShadow = xText + design.shadow.offset.x;
		float yTextShadow = yText + design.shadow.offset.y;

		nvgFontDilate(vg, design.shadow.dilate);
		nvgFontBlur(vg, design.shadow.blur);
		nvgFillColor(vg, design.shadow.color);
		nvgText(vg, xTextShadow, yTextShadow, text, 0);
	}

	nvgFontDilate(vg, 0.0f);
	nvgFontBlur(vg, 0.0f);
	nvgFillColor(vg, design.color);
	nvgText(vg, xText, yText, text, 0);
}

void Mode::drawMarker(MainWindow& window, const LayoutContext& context, int frame, const Design::Marker& marker)
{
	NVGcontext* vg = window.m_vg;

	int c = (int)theApp.totals.size();

	if (frame < 0 || frame >= c)
		return;

	{
		// 垂直線を描画する。

		int x = window.frame2client(frame);
		int my = context.rc.top;
		int ly = context.rc.bottom;

		if (x >= context.graph.left && x <= context.graph.right)
			drawLine(vg, (float)x, (float)my, (float)x, (float)ly, marker.stroke);
	}
}

//--------------------------------------------------------------------

void Mode::drawBackground(MainWindow& window, const LayoutContext& context)
{
	NVGcontext* vg = window.m_vg;

	XYWHRect background;
	background.x = context.x;
	background.y = context.y;
	background.w = context.w;
	background.h = context.h;

	// 縦のグラデーションで背景を塗りつぶす。
	NVGpaint paint = nvgVertGradient(vg, background, g_design.background.fill);
	nvgBeginPath(vg);
	nvgXYWHRect(vg, background);
	nvgFillPaint(vg, paint);
	nvgFill(vg);

	// 背景画像を描画する。
	drawImage(vg, window.m_image, background, g_design.image);
}

void Mode::drawBody(MainWindow& window, const LayoutContext& context)
{
	// ボーダーを描画する。

	NVGcontext* vg = window.m_vg;
	const LayoutContext::Graph& graph = context.graph;

	{
		float mx = (float)(graph.left);
		float my = (float)(context.rc.top);
		float lx = (float)(graph.left);
		float ly = (float)(context.rc.bottom);

		drawLine(vg, mx, my, lx, ly, g_design.body.stroke);
	}

	{
		float mx = (float)(graph.right);
		float my = (float)(context.rc.top);
		float lx = (float)(graph.right);
		float ly = (float)(context.rc.bottom);

		drawLine(vg, mx, my, lx, ly, g_design.body.stroke);
	}

	{
		float mx = (float)(context.rc.left);
		float my = (float)(graph.top);
		float lx = (float)(context.rc.right);
		float ly = (float)(graph.top);

		drawLine(vg, mx, my, lx, ly, g_design.body.stroke);
	}

	{
		float mx = (float)(context.rc.left);
		float my = (float)(graph.bottom);
		float lx = (float)(context.rc.right);
		float ly = (float)(graph.bottom);

		drawLine(vg, mx, my, lx, ly, g_design.body.stroke);
	}
}

void Mode::drawScale(MainWindow& window, const LayoutContext& context)
{
	drawHorzScale(window, context);
	drawVertScale(window, context);
}

void Mode::drawHorzScale(MainWindow& window, const LayoutContext& context)
{
	// 水平目盛りを上下に描画する。

	NVGcontext* vg = window.m_vg;
	const LayoutContext::Graph& graph = context.graph;

	nvgScissor(vg, (float)graph.x, (float)context.y, (float)graph.w, (float)context.h);

	int ox = graph.x - context.hScroll; // 描画範囲の X 座標。
	double unitSec = window.getUnitSec(context.zoomScale);
	double timeBegin = window.px2sec(context.zoomScale, context.hScroll);
	timeBegin /= unitSec;
	timeBegin = floor(timeBegin);
	timeBegin *= unitSec;
	double timeEnd = window.px2sec(context.zoomScale, context.hScroll + graph.w);
	timeEnd /= unitSec;
	timeEnd = ceil(timeEnd);
	timeEnd *= unitSec;

	for (double time = timeBegin; time < timeEnd; time += unitSec)
	{
		int pxTime = ox + window.sec2px(context.zoomScale, time);

		{
			// 長い目盛りを描画する。

			float x = (float)pxTime;

			switch (window.m_horzScaleSettings.lineStyle)
			{
			case MainWindow::HorzScaleSettings::LineStyle::side:
				{
					{
						float my = (float)(graph.top - g_design.scale.horz.primary.height);
						float ly = (float)graph.top;

						drawLine(vg, x, my, x, ly, g_design.scale.horz.primary.stroke);
					}

					{
						float my = (float)(graph.bottom + g_design.scale.horz.primary.height);
						float ly = (float)graph.bottom;

						drawLine(vg, x, my, x, ly, g_design.scale.horz.primary.stroke);
					}

					break;
				}
			case MainWindow::HorzScaleSettings::LineStyle::straight:
				{
					float my = (float)(graph.top - g_design.scale.horz.primary.height);
					float ly = (float)(graph.bottom + g_design.scale.horz.primary.height);

					drawLine(vg, x, my, x, ly, g_design.scale.horz.primary.stroke);

					break;
				}
			}
		}

		{
			// 時間文字列を描画する。

			int hour = (int)time / 60 / 60;
			int min = (int)time / 60 % 60;
			double sec = fmod(time, 60);

			char text[MAX_PATH] = {};
			::StringCbPrintfA(text, sizeof(text), "%02d:%02d:%05.2f", hour, min, sec);

			int padding = 2;

			{
				float xText = (float)(pxTime + padding);
				float yText = (float)(graph.top - g_design.scale.horz.primary.height);

				nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);

				drawText(vg, text, xText, yText, g_design.scale.horz.primary.text);
			}

			{
				float xText = (float)(pxTime + padding);
				float yText = (float)(graph.bottom + g_design.scale.horz.primary.height);

				nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

				drawText(vg, text, xText, yText, g_design.scale.horz.primary.text);
			}
		}

		{
			// 短い目盛りを描画する。

			for (int j = 1; j < 10; j++)
			{
				double subTime = time + unitSec * j / 10.0;
				int pxTime = ox + window.sec2px(context.zoomScale, subTime);
				float x = (float)pxTime;

				switch (window.m_horzScaleSettings.lineStyle)
				{
				case MainWindow::HorzScaleSettings::LineStyle::side:
					{
						{
							float my = (float)(graph.top - g_design.scale.horz.secondary.height);
							float ly = (float)graph.top;

							drawLine(vg, x, my, x, ly, g_design.scale.horz.secondary.stroke);
						}

						{
							float my = (float)(graph.bottom + g_design.scale.horz.secondary.height);
							float ly = (float)graph.bottom;

							drawLine(vg, x, my, x, ly, g_design.scale.horz.secondary.stroke);
						}

						break;
					}
				case MainWindow::HorzScaleSettings::LineStyle::straight:
					{
						float my = (float)(graph.top - g_design.scale.horz.secondary.height);
						float ly = (float)(graph.bottom + g_design.scale.horz.secondary.height);

						drawLine(vg, x, my, x, ly, g_design.scale.horz.secondary.stroke);

						break;
					}
				}
			}
		}
	}

	nvgResetScissor(vg);
}

void Mode::drawVertScale(MainWindow& window, const LayoutContext& context)
{
}

void Mode::drawGraph(MainWindow& window, const LayoutContext& context)
{
	drawBPM(window, context);
	drawMarkers(window, context);
}

void Mode::drawBPM(MainWindow& window, const LayoutContext& context)
{
	NVGcontext* vg = window.m_vg;
	const LayoutContext::Graph& graph = context.graph;

	BOOL showBPM = window.getShowBPM();
	int orig = window.getOrig(); // 基準フレーム番号。
	int bpm = window.getBPM();
	int above = window.getAbove(); // 楽譜に書いてある上の数字。
	int below = window.getBelow(); // 楽譜に書いてある下の数字。

	if (showBPM && bpm > 0 && above > 0 && below > 0)
	{
		nvgScissor(vg, (float)graph.x, (float)graph.y, (float)graph.w, (float)graph.h);

		// BPM を描画する。

		// 1小節の拍数 = (4 * above / below)
		// 1拍の秒数 = (60 / bpm)
		// 1小節の秒数 = (4 * above / below) * (60 / bpm)

		int ox = graph.x - context.hScroll; // 描画範囲の X 座標。
		double unitSec = (4.0 * above / below) * (60.0 / bpm);
		double timeBegin = window.px2sec(context.zoomScale, context.hScroll);
		timeBegin /= unitSec;
		timeBegin = floor(timeBegin);
		timeBegin *= unitSec;
		double timeEnd = window.px2sec(context.zoomScale, context.hScroll + graph.w);
		timeEnd /= unitSec;
		timeEnd = ceil(timeEnd);
		timeEnd *= unitSec;
		double origSec = window.frame2sec(orig);
		double offsetSec = std::fmod(origSec, unitSec);
		int measure = (int)((timeBegin - origSec) / unitSec) - 1;

		for (double time = timeBegin + offsetSec - unitSec; time < timeEnd; time += unitSec)
		{
			int pxTime = ox + window.sec2px(context.zoomScale, time);

			{
				// 長い目盛りを描画する。

				float x = (float)pxTime;
				float my = (float)graph.top;
				float ly = (float)graph.bottom;

				drawLine(vg, x, my, x, ly, g_design.bpm.primary.stroke);
			}

			{
				// 小節文字列を描画する。

				char text[MAX_PATH] = {};
				::StringCbPrintfA(text, sizeof(text), "%d", measure++);

				float xText = (float)(pxTime - 4);
				float yText = (float)(graph.top + 2);

				nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);

				drawText(vg, text, xText, yText, g_design.bpm.primary.text);
			}

			{
				// 短い目盛りを描画する。

				for (int j = 1; j < above; j++)
				{
					double subTime = time + unitSec * j / above;
					int pxTime = ox + window.sec2px(context.zoomScale, subTime);
					float x = (float)pxTime;
					float my = (float)graph.top;
					float ly = (float)graph.bottom;

					drawLine(vg, x, my, x, ly, g_design.bpm.secondary.stroke);
				}
			}
		}

		nvgResetScissor(vg);
	}
}

void Mode::drawMarkers(MainWindow& window, const LayoutContext& context)
{
	NVGcontext* vg = window.m_vg;
	const LayoutContext::Graph& graph = context.graph;

	{
		// カレントフレームを描画する。
		drawMarker(window, context, theApp.projectParams->currentFrame, g_design.graph.current);

		// ホットフレームを描画する。
		drawMarker(window, context, window.m_hotFrame, g_design.graph.hot);
	}

	int c = (int)theApp.totals.size();

	if (c > 0)
	{
		// 最終フレームを表す垂直線を描画する。

		int x = window.frame2client(c - 1);
		int my = context.rc.top;
		int ly = context.rc.bottom;

		if (x >= context.graph.left && x <= context.graph.right)
			drawLine(vg, (float)x, (float)my, (float)x, (float)ly, g_design.graph.last.stroke);
	}
}

void Mode::drawVertScaleLine(MainWindow& window, const LayoutContext& context, float y)
{
	NVGcontext* vg = window.m_vg;
	const LayoutContext::Graph& graph = context.graph;

	switch (window.m_vertScaleSettings.lineStyle)
	{
	case MainWindow::VertScaleSettings::LineStyle::side:
		{
			{
				float mx = (float)graph.left;
				float lx = (float)(graph.left - g_design.scale.vert.width);

				drawLine(vg, mx, y, lx, y, g_design.scale.vert.stroke);
			}

			{
				float mx = (float)graph.right;
				float lx = (float)(graph.right + g_design.scale.vert.width);

				drawLine(vg, mx, y, lx, y, g_design.scale.vert.stroke);
			}

			break;
		}
	case MainWindow::VertScaleSettings::LineStyle::straight:
		{
			float mx = (float)(graph.left - g_design.scale.vert.width);
			float lx = (float)(graph.right + g_design.scale.vert.width);

			drawLine(vg, mx, y, lx, y, g_design.scale.vert.stroke);

			break;
		}
	}
}

void Mode::drawPoints(MainWindow& window, const LayoutContext& context, const std::vector<Mode::PointF>& points)
{
	NVGcontext* vg = window.m_vg;
	const LayoutContext::Graph& graph = context.graph;

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
			(float)graph.left, (float)graph.top, (float)graph.left, (float)graph.bottom,
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
}

//--------------------------------------------------------------------
