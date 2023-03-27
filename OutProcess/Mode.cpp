#include "pch.h"
#include "Mode.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

void Mode::drawBackground(MainWindow& window, const PaintContext& context)
{
	NVGcontext* vg = window.m_vg;

	{
		// 縦のグラデーションで背景を塗りつぶす。
		NVGpaint paint = nvgLinearGradient(vg,
			context.x, context.y, context.x, context.y + context.h,
			g_design.background.fill.color1, g_design.background.fill.color2);
		nvgBeginPath(vg);
		nvgRect(vg, context.x, context.y, context.w, context.h);
		nvgFillPaint(vg, paint);
		nvgFill(vg);
	}

	if (window.m_image)
	{
		// 背景画像を描画する。

		int imgw = 0, imgh = 0;
		nvgImageSize(vg, window.m_image, &imgw, &imgh);

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

		NVGpaint imgPaint = nvgImagePattern(vg,
			(float)dst.x, (float)dst.y, (float)dst.w, (float)dst.h,
			g_design.image.angle / 360.0f * (2.0f * NVG_PI), window.m_image, g_design.image.alpha);
		nvgBeginPath(vg);
		nvgRect(vg, context.x, context.y, context.w, context.h);
		nvgFillPaint(vg, imgPaint);
		nvgFill(vg);
	}
}

void Mode::drawBody(MainWindow& window, const PaintContext& context)
{
	// ボーダーを描画する。

	NVGcontext* vg = window.m_vg;

	nvgStrokeWidth(vg, (float)g_design.body.stroke.width);
	nvgStrokeColor(vg, g_design.body.stroke.color);

	{
		float mx = (float)(context.rc.left + g_design.body.margin);
		float my = (float)(context.rc.top);
		float lx = (float)(context.rc.left + g_design.body.margin);
		float ly = (float)(context.rc.bottom);

		nvgBeginPath(vg);
		nvgMoveTo(vg, mx, my);
		nvgLineTo(vg, lx, ly);
		nvgStroke(vg);
	}

	{
		float mx = (float)(context.rc.right - g_design.body.margin);
		float my = (float)(context.rc.top);
		float lx = (float)(context.rc.right - g_design.body.margin);
		float ly = (float)(context.rc.bottom);

		nvgBeginPath(vg);
		nvgMoveTo(vg, mx, my);
		nvgLineTo(vg, lx, ly);
		nvgStroke(vg);
	}
}

void Mode::drawScale(MainWindow& window, const PaintContext& context)
{
	// ゲージを描画する。

	NVGcontext* vg = window.m_vg;
#if 0
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
	nvgFontFaceId(vg, m_fontDefault);

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

			drawText(tx, ty, text, g_design.scale.text);
		}

		{
			nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM);

			float tx = (float)(context.rc.right - g_design.body.margin + textPadding);
			float ty = y - textPadding;

			drawText(tx, ty, text, g_design.scale.text);
		}
	}
#endif
}

void Mode::drawGraph(MainWindow& window, const PaintContext& context)
{
	NVGcontext* vg = window.m_vg;
#if 0
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
		float level = (fullSamples[i].rms - m_minRange) / (m_maxRange - m_minRange);
		float x = left + lgw * i / (c - 1) - hScroll;
		float y = std::min(bottom, bottom - gh * level);

		if (x >= left && x <= right)
			points.emplace_back(x, y);
	}

	{
		// グラフを塗りつぶす。

		nvgBeginPath(vg);
		nvgMoveTo(vg, gx, bottom + padding);

		for (const auto& point : points)
			nvgLineTo(vg, point.x, point.y);

		nvgLineTo(vg, gx + gw, bottom + padding);
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
		nvgMoveTo(vg, gx, bottom);

		for (const auto& point : points)
			nvgLineTo(vg, point.x, point.y);

		nvgStrokeWidth(vg, (float)g_design.graph.stroke.width);
		nvgStrokeColor(vg, g_design.graph.stroke.color);
		nvgStroke(vg);
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
			nvgBeginPath(vg);
			nvgMoveTo(vg, mx, my);
			nvgLineTo(vg, lx, ly);
			nvgStrokeWidth(vg, (float)g_design.graph.current.stroke.width);
			nvgStrokeColor(vg, g_design.graph.current.stroke.color);
			nvgStroke(vg);
		}

		// テキストを描画する。

		char text[MAX_PATH] = {};
		::StringCbPrintfA(text, sizeof(text), "%.2fdB @%d", fullSamples[frame], frame);

		float padding = 4.0f;
		float tx = left + padding;
		float ty = top + padding;

		nvgFontSize(vg, (float)g_design.graph.current.text.height);
		nvgFontFaceId(vg, m_fontDefault);
		nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

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

		nvgBeginPath(vg);
		nvgMoveTo(vg, mx, my);
		nvgLineTo(vg, lx, ly);
		nvgStrokeWidth(vg, (float)g_design.graph.hot.stroke.width);
		nvgStrokeColor(vg, g_design.graph.hot.stroke.color);
		nvgStroke(vg);

		// テキストを描画する。

		char text[MAX_PATH] = {};
		::StringCbPrintfA(text, sizeof(text), "%.2fdB @%d", fullSamples[frame], frame);

		float padding = 4.0f;
		float tx = right - padding;
		float ty = top + padding;

		nvgFontSize(vg, (float)g_design.graph.hot.text.height);
		nvgFontFaceId(vg, m_fontDefault);
		nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);

		drawText(tx, ty, text, g_design.graph.hot.text);
	}
#endif
}

void Mode::drawText(MainWindow& window, const PaintContext& context, float x, float y, LPCSTR text, const Design::Text& design)
{
	NVGcontext* vg = window.m_vg;

	if (design.shadow.blur != 0.0f)
	{
		nvgFontBlur(vg, design.shadow.blur);
		nvgFillColor(vg, design.shadow.color);
		nvgText(vg, x + design.shadow.offset.x, y + design.shadow.offset.y, text, 0);
	}

	nvgFontBlur(vg, 0.0f);
	nvgFillColor(vg, design.color);
	nvgText(vg, x, y, text, 0);
}

void Mode::drawMarker(MainWindow& window, const PaintContext& context, int frame, float tx, float ty, const Design::Marker& design)
{
	NVGcontext* vg = window.m_vg;

	int c = (int)window.fullSamples.size();

	if (frame < 0 || frame >= c)
		return;

	// 垂直線を描画する。

	float mx = context.left + context.lgw * frame / (c - 1) - context.hScroll;
	float my = context.top;
	float lx = mx;
	float ly = context.bottom;

	if (mx >= context.left && mx <= context.right)
	{
		nvgBeginPath(vg);
		nvgMoveTo(vg, mx, my);
		nvgLineTo(vg, lx, ly);
		nvgStrokeWidth(vg, (float)design.stroke.width);
		nvgStrokeColor(vg, design.stroke.color);
		nvgStroke(vg);
	}

	// テキストを描画する。

	char text[MAX_PATH] = {};
	::StringCbPrintfA(text, sizeof(text), "%.2fdB @%d", window.fullSamples[frame].rms, frame);

	nvgFontSize(vg, (float)design.text.height);
	nvgFontFaceId(vg, window.m_fontDefault);

	drawText(window, context, tx, ty, text, design.text);
}

void Mode::drawMarkers(MainWindow& window, const PaintContext& context)
{
	NVGcontext* vg = window.m_vg;

	float padding = 4.0f;

	// カレントフレームを描画する。
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	drawMarker(window, context, window.projectParams->currentFrame,
		context.left + padding, context.top + padding, g_design.graph.current);

	// ホットフレームを描画する。
	nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
	drawMarker(window, context, window.m_hotFrame,
		context.right - padding, context.top + padding, g_design.graph.hot);
}

//--------------------------------------------------------------------

LRESULT Mode::onMouseMove(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	MY_TRACE(_T("Mode::onMouseMove(0x%08X, 0x%08X)\n"), wParam, lParam);

	POINT point = LP2PT(lParam);

	if (::GetCapture() == hwnd)
	{
		// scale をドラッグして変更する。

		int offset = point.y - m_dragOriginPoint.y;

		window.m_scale = m_dragOriginScale - offset;

		::InvalidateRect(hwnd, 0, FALSE);
	}

	Mode::onMouseMove(window, point);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT Mode::onLButtonDown(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("Mode::onLButtonDown(0x%08X, 0x%08X)\n"), wParam, lParam);

	POINT point = LP2PT(lParam);
	RECT rc; ::GetClientRect(hwnd, &rc);

	if (point.y < rc.top + g_design.body.margin ||
		point.y > rc.bottom - g_design.body.margin)
	{
		// scale のドラッグを開始する。

		::SetCapture(hwnd);
		m_dragOriginPoint = point;
		m_dragOriginScale = window.m_scale;
	}
	else
	{
		Mode::onLButtonDown(window, point);
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT Mode::onLButtonUp(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("Mode::onLButtonUp(0x%08X, 0x%08X)\n"), wParam, lParam);

	::ReleaseCapture();

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------

LRESULT Mode::onMouseMove(MainWindow& window, const POINT& point)
{
	// ホットフレームを更新する。

	int hotFrame = window.px2Frame(point.x);
	if (window.m_hotFrame != hotFrame)
	{
		window.m_hotFrame = hotFrame;

		::InvalidateRect(window.m_hwnd, 0, FALSE);
	}

	return 0;
}

LRESULT Mode::onLButtonDown(MainWindow& window, const POINT& point)
{
	// カレントフレームを変更する。

	int32_t frame = window.px2Frame(point.x);

	::PostMessage(g_parent, WM_AVIUTL_FILTER_CHANGE_FRAME, (WPARAM)frame, 0);

	return 0;
}

//--------------------------------------------------------------------
