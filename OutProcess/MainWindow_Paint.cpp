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

	context.hScroll = ::GetScrollPos(m_hwnd, SB_HORZ);
	context.gx = (float)(context.rc.left + g_design.body.margin);
	context.gy = (float)(context.rc.top);
	context.gw = (float)(context.width - g_design.body.margin * 2);
	context.gh = (float)(context.height);
	context.lgw = context.gw * (100 + m_zoom) / 100;
	context.left = context.gx;
	context.right = context.gx + context.gw;
	context.top = context.gy;
	context.bottom = context.gy + context.gh;
	context.padding = 10.0f;

	m_mode->drawBackground(*this, context);
	m_mode->drawScale(*this, context);
	m_mode->drawBody(*this, context);
	m_mode->drawGraph(*this, context);
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
