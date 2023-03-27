#include "pch.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

void MainWindow::doPaint(HDC dc, const RECT& rc)
{
	MY_TRACE(_T("MainWindow::doPaint(0x%08X)\n"), dc);

	MakeCurrent makeCurrent(dc, m_rc);
	int w = getWidth(rc);
	int h = getHeight(rc);

	MY_TRACE_INT(w);
	MY_TRACE_INT(h);

	glViewport(0, 0, w, h);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	nvgBeginFrame(m_vg, (float)w, (float)h, 1.0f);
	nvgSave(m_vg);

	doPaint(rc);

	nvgRestore(m_vg);
	nvgEndFrame(m_vg);

	::SwapBuffers(dc);
}

void MainWindow::doPaint(const RECT& rc)
{
	if (!projectParams) return;

	PaintContext context;
	context.rc = rc;
	context.width = getWidth(rc);
	context.height = getHeight(rc);

	if (context.width <= 0 || context.height <= 0) return;

	context.x = (float)rc.left;
	context.y = (float)rc.top;
	context.w = (float)context.width;
	context.h = (float)context.height;

	drawBackground(context);
	drawScale(context);
	drawBody(context);
	drawGraph(context);
}

void MainWindow::drawBackground(const PaintContext& context)
{
	{
		// 縦のグラデーションで背景を塗りつぶす。
		NVGpaint paint = nvgLinearGradient(m_vg,
			context.x, context.y, context.x, context.y + context.h,
			g_design.background.fill.color1, g_design.background.fill.color2);
		nvgBeginPath(m_vg);
		nvgRect(m_vg, context.x, context.y, context.w, context.h);
		nvgFillPaint(m_vg, paint);
		nvgFill(m_vg);
	}

	if (m_image)
	{
		// 背景画像を描画する。

		int imgw = 0, imgh = 0;
		nvgImageSize(m_vg, m_image, &imgw, &imgh);

		struct { int x, y, w, h; } dst;
		struct { int x, y, w, h; } src;

		dst.x = src.x = 0;
		dst.y = src.y = 0;
		dst.w = src.w = imgw;
		dst.h = src.h = imgh;

		switch (g_design.image.scaleMode)
		{
		case Design::ScaleMode::fit:
			{
				dst.w = context.width;
				dst.h = context.width * imgh / imgw;

				if (dst.h > context.height)
				{
					dst.w = context.height * imgw / imgh;
					dst.h = context.height;
				}

				break;
			}
		case Design::ScaleMode::crop:
			{
				dst.w = context.width;
				dst.h = context.width * imgh / imgw;

				if (dst.h < context.height)
				{
					dst.w = context.height * imgw / imgh;
					dst.h = context.height;
				}

				break;
			}
		}

		int align = 0;

		switch (g_design.image.align.x)
		{
		case Design::AlignX::left: align |= NVG_ALIGN_LEFT; break;
		case Design::AlignX::right: align |= NVG_ALIGN_RIGHT; break;
		case Design::AlignX::center: align |= NVG_ALIGN_CENTER; break;
		}

		switch (g_design.image.align.y)
		{
		case Design::AlignY::top: align |= NVG_ALIGN_TOP; break;
		case Design::AlignY::bottom: align |= NVG_ALIGN_BOTTOM; break;
		case Design::AlignY::center: align |= NVG_ALIGN_MIDDLE; break;
		}

		if (align & NVG_ALIGN_RIGHT)
		{
			dst.x = context.rc.right - dst.w;
			dst.x -= g_design.image.offset.x;
		}
		else
		{
			if (align & NVG_ALIGN_CENTER)
				dst.x = context.rc.left + (context.width - dst.w) / 2;
	
			dst.x += g_design.image.offset.x;
		}

		if (align & NVG_ALIGN_BOTTOM)
		{
			dst.y = context.rc.bottom - dst.h;
			dst.y -= g_design.image.offset.y;
		}
		else
		{
			if (align & NVG_ALIGN_MIDDLE)
				dst.y = context.rc.top + (context.height - dst.h) / 2;
			
			dst.y += g_design.image.offset.y;
		}

		NVGpaint imgPaint = nvgImagePattern(m_vg,
			(float)dst.x, (float)dst.y, (float)dst.w, (float)dst.h,
			g_design.image.angle / 360.0f * (2.0f * NVG_PI), m_image, g_design.image.alpha);
		nvgBeginPath(m_vg);
		nvgRect(m_vg, context.x, context.y, context.w, context.h);
		nvgFillPaint(m_vg, imgPaint);
		nvgFill(m_vg);
	}
}

void MainWindow::drawBody(const PaintContext& context)
{
	// ボーダーを描画する。

	nvgStrokeWidth(m_vg, (float)g_design.body.stroke.width);
	nvgStrokeColor(m_vg, g_design.body.stroke.color);

	{
		float mx = (float)(context.rc.left + g_design.body.margin);
		float my = (float)(context.rc.top);
		float lx = (float)(context.rc.left + g_design.body.margin);
		float ly = (float)(context.rc.bottom);

		nvgBeginPath(m_vg);
		nvgMoveTo(m_vg, mx, my);
		nvgLineTo(m_vg, lx, ly);
		nvgStroke(m_vg);
	}

	{
		float mx = (float)(context.rc.right - g_design.body.margin);
		float my = (float)(context.rc.top);
		float lx = (float)(context.rc.right - g_design.body.margin);
		float ly = (float)(context.rc.bottom);

		nvgBeginPath(m_vg);
		nvgMoveTo(m_vg, mx, my);
		nvgLineTo(m_vg, lx, ly);
		nvgStroke(m_vg);
	}
}

void MainWindow::drawScale(const PaintContext& context)
{
	// ゲージを描画する。

	int textPadding = 1;
	int textHeight = g_design.scale.text.height + textPadding * 2;

	int range = m_maxRange - m_minRange;
	int freq = range * textHeight / context.height + 1;

	{
		// ベースレベルを描画する。

		float relativeLevel = (float)(m_baseLevel - m_minRange);
		float y = context.rc.top + context.height * (1.0f - relativeLevel / range);

		float mx = (float)(context.rc.left + g_design.body.margin);
		float my = (float)(y);
		float lx = (float)(context.rc.right - g_design.body.margin);
		float ly = (float)(y);

		nvgStrokeWidth(m_vg, (float)g_design.scale.base.stroke.width);
		nvgStrokeColor(m_vg, g_design.scale.base.stroke.color);

		nvgBeginPath(m_vg);
		nvgMoveTo(m_vg, mx, my);
		nvgLineTo(m_vg, lx, ly);
		nvgStroke(m_vg);
	}

	nvgStrokeWidth(m_vg, (float)g_design.scale.stroke.width);
	nvgStrokeColor(m_vg, g_design.scale.stroke.color);
	nvgFontSize(m_vg, (float)g_design.scale.text.height);
	nvgFontFaceId(m_vg, m_fontDefault);

	for (int i = m_maxRange; i > m_minRange - freq; i--)
	{
		if (i % freq) continue;

		float relativeLevel = (float)(i - m_minRange);
		float y = context.rc.top + context.height * (1.0f - relativeLevel / range);

		{
			float mx = (float)(context.rc.left + g_design.body.margin);
			float my = (float)(y);
			float lx = (float)(context.rc.left + g_design.body.margin - g_design.scale.width);
			float ly = (float)(y);

			nvgBeginPath(m_vg);
			nvgMoveTo(m_vg, mx, my);
			nvgLineTo(m_vg, lx, ly);
			nvgStroke(m_vg);
		}

		{
			float mx = (float)(context.rc.right - g_design.body.margin);
			float my = (float)(y);
			float lx = (float)(context.rc.right - g_design.body.margin + g_design.scale.width);
			float ly = (float)(y);

			nvgBeginPath(m_vg);
			nvgMoveTo(m_vg, mx, my);
			nvgLineTo(m_vg, lx, ly);
			nvgStroke(m_vg);
		}

		char text[MAX_PATH] = {};
		::StringCbPrintfA(text, sizeof(text), "%+d", i);

		{
			nvgTextAlign(m_vg, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM);

			float tx = (float)(context.rc.left + g_design.body.margin - textPadding);
			float ty = y - textPadding;

			drawText(tx, ty, text, g_design.scale.text);
		}

		{
			nvgTextAlign(m_vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);

			float tx = (float)(context.rc.right - g_design.body.margin + textPadding);
			float ty = y - textPadding;

			drawText(tx, ty, text, g_design.scale.text);
		}
	}
}

void MainWindow::drawGraph(const PaintContext& context)
{
	int c = (int)fullSamples.size();

	if (c <= 0) return;

	int hScroll = ::GetScrollPos(m_hwnd, SB_HORZ);

	// グラフを描画する。

	float gx = (float)(context.rc.left + g_design.body.margin);
	float gy = (float)(context.rc.top);
	float gw = (float)(context.width - g_design.body.margin * 2);
	float gh = (float)(context.height);
	float lgw = gw * (100 + m_zoom) / 100;
	float left = gx;
	float right = gx + gw;
	float top = gy;
	float bottom = gy + gh;
	float padding = 10.0f;

	struct PointF { float x, y; };
	std::vector<PointF> points;
	for (int i = 0; i < c; i++)
	{
		float level = (fullSamples[i] - m_minRange) / (m_maxRange - m_minRange);
		float x = left + lgw * i / (c - 1) - hScroll;
		float y = std::min(bottom, bottom - gh * level);

		if (x >= left && x <= right)
			points.emplace_back(x, y);
	}

	{
		// グラフを塗りつぶす。

		nvgBeginPath(m_vg);
		nvgMoveTo(m_vg, gx, bottom + padding);

		for (const auto& point : points)
			nvgLineTo(m_vg, point.x, point.y);

		nvgLineTo(m_vg, gx + gw, bottom + padding);
//		nvgClosePath(m_vg);
//		nvgPathWinding(m_vg, NVG_SOLID);

		NVGpaint paint = nvgLinearGradient(m_vg,
			context.x, context.y, context.x, context.y + context.h,
			g_design.graph.fill.color1, g_design.graph.fill.color2);
		nvgFillPaint(m_vg, paint);
		nvgFill(m_vg);
	}

	{
		// グラフのストロークを描画する。

		nvgBeginPath(m_vg);
		nvgMoveTo(m_vg, gx, bottom);

		for (const auto& point : points)
			nvgLineTo(m_vg, point.x, point.y);

		nvgStrokeWidth(m_vg, (float)g_design.graph.stroke.width);
		nvgStrokeColor(m_vg, g_design.graph.stroke.color);
		nvgStroke(m_vg);
	}

	int frame = projectParams->currentFrame;

	if (frame >= 0 && frame < c)
	{
		// カレントフレームを描画する。

		// 垂直線を描画する。

		float mx = left + lgw * frame / (c - 1) - hScroll;
		float my = top;
		float lx = mx;
		float ly = bottom;

		if (mx >= left && mx <= right)
		{
			nvgBeginPath(m_vg);
			nvgMoveTo(m_vg, mx, my);
			nvgLineTo(m_vg, lx, ly);
			nvgStrokeWidth(m_vg, (float)g_design.graph.current.stroke.width);
			nvgStrokeColor(m_vg, g_design.graph.current.stroke.color);
			nvgStroke(m_vg);
		}

		// テキストを描画する。

		char text[MAX_PATH] = {};
		::StringCbPrintfA(text, sizeof(text), "%.2fdB @%d", fullSamples[frame], frame);

		float padding = 4.0f;
		float tx = left + padding;
		float ty = top + padding;

		nvgFontSize(m_vg, (float)g_design.graph.current.text.height);
		nvgFontFaceId(m_vg, m_fontDefault);
		nvgTextAlign(m_vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

		drawText(tx, ty, text, g_design.graph.current.text);
	}

	frame = m_hotFrame;

	if (frame >= 0 && frame < c)
	{
		// ホットフレームを描画する。

		// 垂直線を描画する。

		float mx = left + lgw * frame / (c - 1) - hScroll;
		float my = top;
		float lx = mx;
		float ly = bottom;

		nvgBeginPath(m_vg);
		nvgMoveTo(m_vg, mx, my);
		nvgLineTo(m_vg, lx, ly);
		nvgStrokeWidth(m_vg, (float)g_design.graph.hot.stroke.width);
		nvgStrokeColor(m_vg, g_design.graph.hot.stroke.color);
		nvgStroke(m_vg);

		// テキストを描画する。

		char text[MAX_PATH] = {};
		::StringCbPrintfA(text, sizeof(text), "%.2fdB @%d", fullSamples[frame], frame);

		float padding = 4.0f;
		float tx = right - padding;
		float ty = top + padding;

		nvgFontSize(m_vg, (float)g_design.graph.hot.text.height);
		nvgFontFaceId(m_vg, m_fontDefault);
		nvgTextAlign(m_vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);

		drawText(tx, ty, text, g_design.graph.hot.text);
	}
}

void MainWindow::drawText(float x, float y, LPCSTR text, const Design::Text& design)
{
	if (design.shadow.blur != 0.0f)
	{
		nvgFontBlur(m_vg, design.shadow.blur);
		nvgFillColor(m_vg, design.shadow.color);
		nvgText(m_vg, x + design.shadow.offset.x, y + design.shadow.offset.y, text, 0);
	}

	nvgFontBlur(m_vg, 0.0f);
	nvgFillColor(m_vg, design.color);
	nvgText(m_vg, x, y, text, 0);
}

//--------------------------------------------------------------------

LRESULT MainWindow::onPaint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PaintDC dc(hwnd);
	RECT rc = dc.m_ps.rcPaint;
	doPaint(dc, rc);

	return 0;
}

//--------------------------------------------------------------------
