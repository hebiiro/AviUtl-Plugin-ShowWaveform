#pragma once

#include "OutProcess.h"
#include "Design.h"

//--------------------------------------------------------------------

struct MainWindow;

struct PaintContext {
	RECT rc;
	int width, height;
	float x, y, w, h;
	int hScroll;
	float gx, gy, gw, gh, lgw;
	float left, right, top, bottom;
	float padding;
};

//--------------------------------------------------------------------

struct Mode
{
	POINT m_dragOriginPoint = {};
	int m_dragOriginScale = 0;

	virtual int getID() const = 0;

	virtual void drawBackground(MainWindow& window, const PaintContext& context);
	virtual void drawScale(MainWindow& window, const PaintContext& context);
	virtual void drawBody(MainWindow& window, const PaintContext& context);
	virtual void drawGraph(MainWindow& window, const PaintContext& context);
	virtual void drawText(MainWindow& window, const PaintContext& context, float x, float y, LPCSTR text, const Design::Text& design);
	virtual void drawMarker(MainWindow& window, const PaintContext& context, int frame, float tx, float ty, const Design::Marker& design);
	virtual void drawMarkers(MainWindow& window, const PaintContext& context);

	virtual LRESULT onMouseMove(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT onLButtonDown(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT onLButtonUp(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual LRESULT onMouseMove(MainWindow& window, const POINT& point);
	virtual LRESULT onLButtonDown(MainWindow& window, const POINT& point);
};

struct MinMode : public Mode
{
	virtual int getID() const;

	virtual void drawScale(MainWindow& window, const PaintContext& context);
	virtual void drawGraph(MainWindow& window, const PaintContext& context);
};

struct MaxMode : public Mode
{
	virtual int getID() const;

	virtual void drawScale(MainWindow& window, const PaintContext& context);
	virtual void drawGraph(MainWindow& window, const PaintContext& context);

	virtual LRESULT onMouseMove(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

struct BothMode : public Mode
{
	virtual int getID() const;

	virtual void drawScale(MainWindow& window, const PaintContext& context);
	virtual void drawGraph(MainWindow& window, const PaintContext& context);
};

struct RMSMode : public Mode
{
	struct DragMode {
		static const int minRange = 0;
		static const int maxRange = 1;
	};

	int m_dragMode = DragMode::minRange;
	POINT m_dragOriginPoint = {};
	int m_dragOriginRange = 0;

	virtual int getID() const;

	virtual void drawScale(MainWindow& window, const PaintContext& context);
	virtual void drawGraph(MainWindow& window, const PaintContext& context);

	virtual LRESULT onMouseMove(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT onLButtonDown(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT onLButtonUp(MainWindow& window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

typedef std::shared_ptr<Mode> ModePtr;

//--------------------------------------------------------------------
