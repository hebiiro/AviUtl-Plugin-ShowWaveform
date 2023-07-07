#pragma once

#include "OutProcess.h"
#include "Design.h"

//--------------------------------------------------------------------

struct MainWindow;
struct LayoutContext;

//--------------------------------------------------------------------

struct Mode
{
	virtual int getID() const = 0;

	static void nvgStrokeDesign(NVGcontext* vg, const Design::Stroke& stroke);
	static void drawLine(NVGcontext* vg, float mx, float my, float lx, float ly, const Design::Stroke& stroke);
	static void drawText(NVGcontext* vg, LPCSTR text, float x, float y, const Design::Text& design);
	static void drawMarker(MainWindow& window, const LayoutContext& context, int frame, const Design::Marker& marker);

	virtual void drawBackground(MainWindow& window, const LayoutContext& context);
	virtual void drawBody(MainWindow& window, const LayoutContext& context);
	virtual void drawScale(MainWindow& window, const LayoutContext& context);
	virtual void drawHorzScale(MainWindow& window, const LayoutContext& context);
	virtual void drawVertScale(MainWindow& window, const LayoutContext& context);
	virtual void drawGraph(MainWindow& window, const LayoutContext& context);

	struct PointF { float x, y; };

	void drawBPM(MainWindow& window, const LayoutContext& context);
	void drawMarkers(MainWindow& window, const LayoutContext& context);
	void drawVertScaleLine(MainWindow& window, const LayoutContext& context, float y);
	void drawPoints(MainWindow& window, const LayoutContext& context, const std::vector<PointF>& points);
};

struct RMSMode : public Mode
{
	virtual int getID() const;

	virtual void drawVertScale(MainWindow& window, const LayoutContext& context);
	virtual void drawGraph(MainWindow& window, const LayoutContext& context);
};

struct CenterMode : public Mode
{
	virtual int getID() const;

	virtual void drawVertScale(MainWindow& window, const LayoutContext& context);
	virtual void drawGraph(MainWindow& window, const LayoutContext& context);
};

struct BottomUpMode : public Mode
{
	virtual int getID() const;

	virtual void drawVertScale(MainWindow& window, const LayoutContext& context);
	virtual void drawGraph(MainWindow& window, const LayoutContext& context);
};

struct TopDownMode : public Mode
{
	virtual int getID() const;

	virtual void drawVertScale(MainWindow& window, const LayoutContext& context);
	virtual void drawGraph(MainWindow& window, const LayoutContext& context);
};

typedef std::shared_ptr<Mode> ModePtr;

//--------------------------------------------------------------------
