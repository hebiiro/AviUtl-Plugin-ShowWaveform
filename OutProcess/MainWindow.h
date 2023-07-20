#pragma once

#include "OutProcess.h"
#include "Design.h"
#include "Mode.h"

//--------------------------------------------------------------------

struct MakeCurrent
{
	HDC m_prevDC = 0;
	HGLRC m_prevRC = 0;

	MakeCurrent(HDC dc, HGLRC rc)
	{
		m_prevDC = wglGetCurrentDC();
		m_prevRC = wglGetCurrentContext();

		wglMakeCurrent(dc, rc);
	}

	~MakeCurrent()
	{
		wglMakeCurrent(m_prevDC, m_prevRC);
	}
};

struct LayoutContext {
	RECT rc;
	int x, y, w, h;
	int hScroll;
	struct Graph {
		int x, y, w, h;
		int left, right, top, bottom;
	} graph;
	int padding;
	double zoomScale;
};

//--------------------------------------------------------------------

struct MainWindow
{
	struct CommandID {
		struct Mode {
			static const UINT rms = 1000;
			static const UINT center = 1001;
			static const UINT bottomUp = 1002;
			static const UINT topDown = 1003;
		};
		struct HorzScale {
			struct VisibleStyle {
				static const UINT both = 2000;
				static const UINT top = 2001;
				static const UINT bottom = 2002;
			};
			struct LineStyle {
				static const UINT side = 2100;
				static const UINT straight = 2101;
			};
		};
		struct VertScale {
			struct VisibleStyle {
				static const UINT both = 3000;
				static const UINT left = 3001;
				static const UINT right = 3002;
			};
			struct LineStyle {
				static const UINT side = 3100;
				static const UINT straight = 3101;
			};
		};
		static const UINT showMainDialog = 4000;
	};

	struct TimerID {
		static const UINT checkConfig = 1000;
	};

	struct Mode {
		static const int rms = 0;
		static const int center = 1;
		static const int bottomUp = 2;
		static const int topDown = 3;
		static const Label labels[];
	};

	struct HorzScaleSettings {
		struct VisibleStyle {
			static const int both = 0;
			static const int top = 1;
			static const int bottom = 2;
			static const Label labels[];
		};
		struct LineStyle {
			static const int side = 0;
			static const int straight = 1;
			static const Label labels[];
		};
		int visibleStyle;
		int lineStyle;
	};

	struct VertScaleSettings {
		struct VisibleStyle {
			static const int both = 0;
			static const int left = 1;
			static const int right = 2;
			static const Label labels[];
		};
		struct LineStyle {
			static const int side = 0;
			static const int straight = 1;
			static const Label labels[];
		};
		int visibleStyle;
		int lineStyle;
	};

	struct HitTest {
		static const int None = 0;
		static const int HorzScale = 1;
		static const int VertScaleMin = 2;
		static const int VertScaleMax = 3;
	};

	int m_limitVolume = 100;
	int m_baseVolume = 50;
	int m_minRMS = -33;
	int m_maxRMS = 14;
	int m_baseRMS = 0;
	int m_zoom = 0; // グラフの論理幅を増加させる。
	HorzScaleSettings m_horzScaleSettings = {
		HorzScaleSettings::VisibleStyle::both,
		HorzScaleSettings::LineStyle::side,
	};
	VertScaleSettings m_vertScaleSettings = {
		VertScaleSettings::VisibleStyle::both,
		VertScaleSettings::LineStyle::straight,
	};
	TotalsParams m_totalsParams;
	ModePtr m_mode = std::make_shared<RMSMode>();

	int m_hotFrame = 0;
	struct Drag {
		int ht = HitTest::None;
		POINT origPoint = {};
		int origValue = 0;
	} drag;

	BOOL getShowBPM();
	BOOL setShowBPM(BOOL newShowBPM);
	int getOrig();
	BOOL setOrig(int newOrig);
	int getBPM();
	BOOL setBPM(int newBPM);
	int getAbove();
	BOOL setAbove(int newAbove);
	int getBelow();
	BOOL setBelow(int newBelow);

	int getLimitVolume();
	BOOL setLimitVolume(int newLimitVolume);
	int getBaseVolume();
	BOOL setBaseVolume(int newBaseVolume);
	int getMinRMS();
	BOOL setMinRMS(int newMinRMS);
	int getMaxRMS();
	BOOL setMaxRMS(int newMaxRMS);
	int getBaseRMS();
	BOOL setBaseRMS(int newBaseRMS);
	int getZoom();
	BOOL setZoom(int newZoom);
	ModePtr getMode();
	int getHotFrame();
	BOOL setHotFrame(int newHotFrame);
	BOOL updateShared();

	// Layout

	double getZoomScale();
	BOOL getLayoutContext(LayoutContext& context);
	BOOL getLayoutContext(LayoutContext& context, const RECT& rc);
	void recalcLayout();
	int hitTest(POINT point);
	int client2frame(int x);
	int client2frame(const LayoutContext& context, int x);
	int frame2client(int frame);
	int frame2client(const LayoutContext& context, int frame);
	int sec2frame(double sec);
	double frame2sec(int frame);
	static int sec2px(double zoomScale, double sec);
	static double px2sec(double zoomScale, int px);
	static double getUnitSec(double zoomScale);
	void outputFrames();

	// Config

	FileUpdateCheckerPtr m_configFileChecker;
	BOOL initConfig();
	BOOL termConfig();
	void reloadConfig();
	void recalcConfig();
	void recalcConfigInternal();
	LRESULT onTimerCheckConfig(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	void load(LPCWSTR fileName);
	void save(LPCWSTR fileName);

	// OpenGL & NanoVG

	HGLRC m_rc = 0;
	NVGcontext* m_vg = 0;
	int m_fontDefault = 0;
	int m_fontDefault2 = 0;
	int m_image = 0;
	static BOOL setupPixelFormat(HDC dc);
	BOOL initOpenGL();
	BOOL termOpenGL();
	void doPaint(HDC dc, const RECT& rc);
	void doPaint(const RECT& rc);

	// Window

	HWND m_hwnd = 0;
	operator HWND() const { return m_hwnd; }
	void redraw();
	BOOL create(HINSTANCE instance, HWND parent);
	LRESULT onCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onDestroy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onSize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onHScroll(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onTimer(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onPaint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onContextMenu(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onSetCursor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onMouseMove(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onMouseWheel(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onLButtonDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onLButtonUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterExit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterResize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

//--------------------------------------------------------------------
