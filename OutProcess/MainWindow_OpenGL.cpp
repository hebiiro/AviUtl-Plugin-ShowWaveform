#include "pch.h"
#include "MainWindow.h"

#define NANOVG_GL2_IMPLEMENTATION
#include "NanoVG/nanovg_gl.h"

//--------------------------------------------------------------------

BOOL MainWindow::setupPixelFormat(HDC dc)
{
	MY_TRACE(_T("MainWindow::setupPixelFormat(0x%08X)\n"), dc);

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(pfd),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,
		1,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int pixelFormat = ::ChoosePixelFormat(dc, &pfd);
	MY_TRACE_INT(pixelFormat);

	if (!pixelFormat)
	{
		MY_TRACE(_T("::ChoosePixelFormat() が失敗しました\n"));

		return FALSE;
	}

	if (!::SetPixelFormat(dc, pixelFormat, &pfd))
	{
		MY_TRACE(_T("::SetPixelFormat() が失敗しました\n"));

		return FALSE;
	}

	return TRUE;
}

BOOL MainWindow::initOpenGL()
{
	MY_TRACE(_T("MainWindow::initOpenGL()\n"));

	// OpenGL を初期化する。

	ClientDC dc(m_hwnd);

	setupPixelFormat(dc);

	m_rc = wglCreateContext(dc);
	MY_TRACE_HEX(m_rc);

	if (!m_rc)
	{
		MY_TRACE(_T("wglCreateContext() が失敗しました\n"));

		return -1;
	}

	MakeCurrent makeCurrent(dc, m_rc);

	// glad を初期化する。

	gladLoaderLoadGL();

	// NanoVG を初期化する。

	m_vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	MY_TRACE_HEX(m_vg);

	return TRUE;
}

BOOL MainWindow::termOpenGL()
{
	MY_TRACE(_T("MainWindow::termOpenGL()\n"));

	nvgDeleteGL2(m_vg), m_vg = 0;

	wglDeleteContext(m_rc), m_rc = 0;

	return TRUE;
}

//--------------------------------------------------------------------
