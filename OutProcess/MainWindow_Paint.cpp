#include "pch.h"
#include "MainWindow.h"
#include "App.h"

//--------------------------------------------------------------------

void MainWindow::doPaint(HDC dc, const RECT& rc)
{
	MY_TRACE(_T("MainWindow::doPaint(0x%08X)\n"), dc);

	MakeCurrent makeCurrent(dc, m_rc);
	int w = getWidth(rc);
	int h = getHeight(rc);

	glViewport(0, 0, w, h);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	nvgBeginFrame(m_vg, (float)w, (float)h, 1.0f);
	nvgSave(m_vg);

	nvgFontFaceId(m_vg, m_fontDefault);

	doPaint(rc);

	nvgRestore(m_vg);
	nvgEndFrame(m_vg);

	::SwapBuffers(dc);
}

void MainWindow::doPaint(const RECT& rc)
{
	if (!theApp.projectParams) return;

	LayoutContext context = {};
	if (!getLayoutContext(context, rc)) return;

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
