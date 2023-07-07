#include "pch.h"
#include "Design.h"

//--------------------------------------------------------------------

Design g_design;

//--------------------------------------------------------------------

const Label Design::AlignX::labels[] = {
	{ left, L"left" },
	{ right, L"right" },
	{ center, L"center" },
};

const Label Design::AlignY::labels[] = {
	{ top, L"top" },
	{ bottom, L"bottom" },
	{ center, L"center" },
};

const Label Design::ScaleMode::labels[] = {
	{ normal, L"normal" },
	{ fit, L"fit" },
	{ crop, L"crop" },
};

const Label Design::LineStyle::labels[] = {
	{ 0, L"normal" },
	{ NVG_LINE_SOLID, L"solid" },
	{ NVG_LINE_DASHED, L"dashed" },
	{ NVG_LINE_DOTTED, L"dotted" },
	{ NVG_LINE_GLOW, L"glow" },
};

//--------------------------------------------------------------------

Design::Design()
{
	fontDefault = L"C:\\Windows\\Fonts\\segoeui.ttf";
	fontDefault2 = L"C:\\Windows\\Fonts\\meiryo.ttc";
	image.fileName = L"";
	image.offset.x = 0;
	image.offset.y = 0;
	image.align.x = AlignX::center;
	image.align.y = AlignY::center;
	image.scaleMode = ScaleMode::normal;
	image.scale = 1.0f;
	image.angle = 0.0f;
	image.alpha = 1.0f;
	background.fill.color1 = nvgRGBAf(0.2f, 0.2f, 0.2f, 1.0f);
	background.fill.color2 = nvgRGBAf(0.1f, 0.1f, 0.1f, 1.0f);
	body.margin.x = 40;
	body.margin.y = 30;
	body.stroke.style = NVG_LINE_SOLID;
	body.stroke.width = 1;
	body.stroke.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.8f);
	scale.horz.minUnitWidth = 100;
	scale.horz.primary.height = 10;
	scale.horz.primary.stroke.style = NVG_LINE_SOLID;
	scale.horz.primary.stroke.width = 1;
	scale.horz.primary.stroke.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.8f);
	scale.horz.primary.text.height = 16;
	scale.horz.primary.text.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.8f);
	scale.horz.primary.text.shadow.dilate = 1.0f;
	scale.horz.primary.text.shadow.blur = 1.0f;
	scale.horz.primary.text.shadow.offset = { 0, 0 };
	scale.horz.primary.text.shadow.color = nvgRGBAf(0.0f, 0.0f, 0.0f, 0.5f);
	scale.horz.secondary.height = 5;
	scale.horz.secondary.stroke.style = NVG_LINE_SOLID;
	scale.horz.secondary.stroke.width = 1;
	scale.horz.secondary.stroke.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.4f);
	scale.vert.width = 10;
	scale.vert.text.height = 16;
	scale.vert.text.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.8f);
	scale.vert.text.shadow.dilate = 1.0f;
	scale.vert.text.shadow.blur = 1.0f;
	scale.vert.text.shadow.offset = { 0, 0 };
	scale.vert.text.shadow.color = nvgRGBAf(0.0f, 0.0f, 0.0f, 0.5f);
	scale.vert.stroke.style = NVG_LINE_SOLID;
	scale.vert.stroke.width = 1;
	scale.vert.stroke.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.4f);
	scale.vert.base.stroke.style = NVG_LINE_SOLID;
	scale.vert.base.stroke.width = 1;
	scale.vert.base.stroke.color = nvgRGBAf(1.0f, 0.0f, 0.0f, 0.5f);
	graph.fill.color1 = nvgRGBAf(1.0f, 1.0f, 0.0f, 0.4f);
	graph.fill.color2 = nvgRGBAf(1.0f, 1.0f, 0.0f, 0.4f);
	graph.stroke.style = NVG_LINE_SOLID;
	graph.stroke.width = 1;
	graph.stroke.color = nvgRGBAf(1.0f, 1.0f, 0.0f, 0.2f);
	graph.current.stroke.style = NVG_LINE_SOLID;
	graph.current.stroke.width = 1;
	graph.current.stroke.color = nvgRGBAf(0.0f, 1.0f, 0.0f, 0.4f);
	graph.hot.stroke.style = NVG_LINE_SOLID;
	graph.hot.stroke.width = 1;
	graph.hot.stroke.color = nvgRGBAf(1.0f, 0.0f, 1.0f, 0.4f);
	graph.last.stroke.style = NVG_LINE_SOLID;
	graph.last.stroke.width = 1;
	graph.last.stroke.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.5f);
	bpm.primary.stroke.style = NVG_LINE_SOLID;
	bpm.primary.stroke.width = 1;
	bpm.primary.stroke.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.5f);
	bpm.primary.text.height = 12;
	bpm.primary.text.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.8f);
	bpm.primary.text.shadow.dilate = 1.0f;
	bpm.primary.text.shadow.blur = 1.0f;
	bpm.primary.text.shadow.offset = { 0, 0 };
	bpm.primary.text.shadow.color = nvgRGBAf(0.0f, 0.0f, 0.0f, 0.5f);
	bpm.secondary.stroke.style = NVG_LINE_DASHED;
	bpm.secondary.stroke.width = 1;
	bpm.secondary.stroke.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.5f);
}

Design::~Design()
{
}

void Design::load(LPCWSTR fileName)
{
	getPrivateProfileBSTR(fileName, L"Design", L"fontDefault", fontDefault);
	getPrivateProfileBSTR(fileName, L"Design", L"fontDefault2", fontDefault2);

	getPrivateProfileBSTR(fileName, L"Design", L"image.fileName", image.fileName);
	getPrivateProfileInt(fileName, L"Design", L"image.offset.x", image.offset.x);
	getPrivateProfileInt(fileName, L"Design", L"image.offset.y", image.offset.y);
	getPrivateProfileLabel(fileName, L"Design", L"image.align.x", image.align.x, AlignX::labels);
	getPrivateProfileLabel(fileName, L"Design", L"image.align.y", image.align.y, AlignY::labels);
	getPrivateProfileLabel(fileName, L"Design", L"image.scaleMode", image.scaleMode, ScaleMode::labels);
	getPrivateProfileReal(fileName, L"Design", L"image.scale", image.scale);
	getPrivateProfileReal(fileName, L"Design", L"image.angle", image.angle);
	getPrivateProfileReal(fileName, L"Design", L"image.alpha", image.alpha);

	getPrivateProfileColor(fileName, L"Design", L"background.fill.color1", background.fill.color1);
	getPrivateProfileColor(fileName, L"Design", L"background.fill.color2", background.fill.color2);

	getPrivateProfileInt(fileName, L"Design", L"body.margin.x", body.margin.x);
	getPrivateProfileInt(fileName, L"Design", L"body.margin.y", body.margin.y);
	getPrivateProfileLabel(fileName, L"Design", L"body.stroke.style", body.stroke.style, LineStyle::labels);
	getPrivateProfileInt(fileName, L"Design", L"body.stroke.width", body.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"body.stroke.color", body.stroke.color);

	getPrivateProfileInt(fileName, L"Design", L"scale.horz.minUnitWidth", scale.horz.minUnitWidth);
	getPrivateProfileInt(fileName, L"Design", L"scale.horz.primary.height", scale.horz.primary.height);
	getPrivateProfileLabel(fileName, L"Design", L"scale.horz.primary.stroke.style", scale.horz.primary.stroke.style, LineStyle::labels);
	getPrivateProfileInt(fileName, L"Design", L"scale.horz.primary.stroke.width", scale.horz.primary.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"scale.horz.primary.stroke.color", scale.horz.primary.stroke.color);
	getPrivateProfileInt(fileName, L"Design", L"scale.horz.primary.text.height", scale.horz.primary.text.height);
	getPrivateProfileColor(fileName, L"Design", L"scale.horz.primary.text.color", scale.horz.primary.text.color);
	getPrivateProfileReal(fileName, L"Design", L"scale.horz.primary.text.shadow.dilate", scale.horz.primary.text.shadow.dilate);
	getPrivateProfileReal(fileName, L"Design", L"scale.horz.primary.text.shadow.blur", scale.horz.primary.text.shadow.blur);
	getPrivateProfileInt(fileName, L"Design", L"scale.horz.primary.text.shadow.offset.x", scale.horz.primary.text.shadow.offset.x);
	getPrivateProfileInt(fileName, L"Design", L"scale.horz.primary.text.shadow.offset.y", scale.horz.primary.text.shadow.offset.y);
	getPrivateProfileColor(fileName, L"Design", L"scale.horz.primary.text.shadow.color", scale.horz.primary.text.shadow.color);
	getPrivateProfileInt(fileName, L"Design", L"scale.horz.secondary.height", scale.horz.secondary.height);
	getPrivateProfileLabel(fileName, L"Design", L"scale.horz.secondary.stroke.style", scale.horz.secondary.stroke.style, LineStyle::labels);
	getPrivateProfileInt(fileName, L"Design", L"scale.horz.secondary.stroke.width", scale.horz.secondary.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"scale.horz.secondary.stroke.color", scale.horz.secondary.stroke.color);

	getPrivateProfileInt(fileName, L"Design", L"scale.vert.width", scale.vert.width);
	getPrivateProfileInt(fileName, L"Design", L"scale.vert.text.height", scale.vert.text.height);
	getPrivateProfileColor(fileName, L"Design", L"scale.vert.text.color", scale.vert.text.color);
	getPrivateProfileReal(fileName, L"Design", L"scale.vert.text.shadow.dilate", scale.vert.text.shadow.dilate);
	getPrivateProfileReal(fileName, L"Design", L"scale.vert.text.shadow.blur", scale.vert.text.shadow.blur);
	getPrivateProfileInt(fileName, L"Design", L"scale.vert.text.shadow.offset.x", scale.vert.text.shadow.offset.x);
	getPrivateProfileInt(fileName, L"Design", L"scale.vert.text.shadow.offset.y", scale.vert.text.shadow.offset.y);
	getPrivateProfileColor(fileName, L"Design", L"scale.vert.text.shadow.color", scale.vert.text.shadow.color);
	getPrivateProfileLabel(fileName, L"Design", L"scale.vert.stroke.style", scale.vert.stroke.style, LineStyle::labels);
	getPrivateProfileInt(fileName, L"Design", L"scale.vert.stroke.width", scale.vert.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"scale.vert.stroke.color", scale.vert.stroke.color);
	getPrivateProfileLabel(fileName, L"Design", L"scale.vert.base.stroke.style", scale.vert.base.stroke.style, LineStyle::labels);
	getPrivateProfileInt(fileName, L"Design", L"scale.vert.base.stroke.width", scale.vert.base.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"scale.vert.base.stroke.color", scale.vert.base.stroke.color);

	getPrivateProfileColor(fileName, L"Design", L"graph.fill.color1", graph.fill.color1);
	getPrivateProfileColor(fileName, L"Design", L"graph.fill.color2", graph.fill.color2);
	getPrivateProfileLabel(fileName, L"Design", L"graph.stroke.style", graph.stroke.style, LineStyle::labels);
	getPrivateProfileInt(fileName, L"Design", L"graph.stroke.width", graph.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"graph.stroke.color", graph.stroke.color);
	getPrivateProfileLabel(fileName, L"Design", L"graph.current.stroke.style", graph.current.stroke.style, LineStyle::labels);
	getPrivateProfileInt(fileName, L"Design", L"graph.current.stroke.width", graph.current.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"graph.current.stroke.color", graph.current.stroke.color);
	getPrivateProfileLabel(fileName, L"Design", L"graph.hot.stroke.style", graph.hot.stroke.style, LineStyle::labels);
	getPrivateProfileInt(fileName, L"Design", L"graph.hot.stroke.width", graph.hot.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"graph.hot.stroke.color", graph.hot.stroke.color);
	getPrivateProfileLabel(fileName, L"Design", L"graph.last.stroke.style", graph.last.stroke.style, LineStyle::labels);
	getPrivateProfileInt(fileName, L"Design", L"graph.last.stroke.width", graph.last.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"graph.last.stroke.color", graph.last.stroke.color);

	getPrivateProfileLabel(fileName, L"Design", L"bpm.primary.stroke.style", bpm.primary.stroke.style, LineStyle::labels);
	getPrivateProfileInt(fileName, L"Design", L"bpm.primary.stroke.width", bpm.primary.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"bpm.primary.stroke.color", bpm.primary.stroke.color);
	getPrivateProfileInt(fileName, L"Design", L"bpm.primary.text.height", bpm.primary.text.height);
	getPrivateProfileColor(fileName, L"Design", L"bpm.primary.text.color", bpm.primary.text.color);
	getPrivateProfileReal(fileName, L"Design", L"bpm.primary.text.shadow.dilate", bpm.primary.text.shadow.dilate);
	getPrivateProfileReal(fileName, L"Design", L"bpm.primary.text.shadow.blur", bpm.primary.text.shadow.blur);
	getPrivateProfileInt(fileName, L"Design", L"bpm.primary.text.shadow.offset.x", bpm.primary.text.shadow.offset.x);
	getPrivateProfileInt(fileName, L"Design", L"bpm.primary.text.shadow.offset.y", bpm.primary.text.shadow.offset.y);
	getPrivateProfileColor(fileName, L"Design", L"bpm.primary.text.shadow.color", bpm.primary.text.shadow.color);
	getPrivateProfileLabel(fileName, L"Design", L"bpm.secondary.stroke.style", bpm.secondary.stroke.style, LineStyle::labels);
	getPrivateProfileInt(fileName, L"Design", L"bpm.secondary.stroke.width", bpm.secondary.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"bpm.secondary.stroke.color", bpm.secondary.stroke.color);
}

void Design::save(LPCWSTR fileName)
{
	setPrivateProfileBSTR(fileName, L"Design", L"fontDefault", fontDefault);
	setPrivateProfileBSTR(fileName, L"Design", L"fontDefault2", fontDefault2);

	setPrivateProfileBSTR(fileName, L"Design", L"image.fileName", image.fileName);
	setPrivateProfileInt(fileName, L"Design", L"image.offset.x", image.offset.x);
	setPrivateProfileInt(fileName, L"Design", L"image.offset.y", image.offset.y);
	setPrivateProfileLabel(fileName, L"Design", L"image.align.x", image.align.x, AlignX::labels);
	setPrivateProfileLabel(fileName, L"Design", L"image.align.y", image.align.y, AlignY::labels);
	setPrivateProfileLabel(fileName, L"Design", L"image.scaleMode", image.scaleMode, ScaleMode::labels);
	setPrivateProfileReal(fileName, L"Design", L"image.scale", image.scale);
	setPrivateProfileReal(fileName, L"Design", L"image.angle", image.angle);
	setPrivateProfileReal(fileName, L"Design", L"image.alpha", image.alpha);

	setPrivateProfileColor(fileName, L"Design", L"background.fill.color1", background.fill.color1);
	setPrivateProfileColor(fileName, L"Design", L"background.fill.color2", background.fill.color2);

	setPrivateProfileInt(fileName, L"Design", L"body.margin.x", body.margin.x);
	setPrivateProfileInt(fileName, L"Design", L"body.margin.y", body.margin.y);
	setPrivateProfileLabel(fileName, L"Design", L"body.stroke.style", body.stroke.style, LineStyle::labels);
	setPrivateProfileInt(fileName, L"Design", L"body.stroke.width", body.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"body.stroke.color", body.stroke.color);

	setPrivateProfileInt(fileName, L"Design", L"scale.horz.minUnitWidth", scale.horz.minUnitWidth);
	setPrivateProfileInt(fileName, L"Design", L"scale.horz.primary.height", scale.horz.primary.height);
	setPrivateProfileLabel(fileName, L"Design", L"scale.horz.primary.stroke.style", scale.horz.primary.stroke.style, LineStyle::labels);
	setPrivateProfileInt(fileName, L"Design", L"scale.horz.primary.stroke.width", scale.horz.primary.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"scale.horz.primary.stroke.color", scale.horz.primary.stroke.color);
	setPrivateProfileInt(fileName, L"Design", L"scale.horz.primary.text.height", scale.horz.primary.text.height);
	setPrivateProfileColor(fileName, L"Design", L"scale.horz.primary.text.color", scale.horz.primary.text.color);
	setPrivateProfileReal(fileName, L"Design", L"scale.horz.primary.text.shadow.dilate", scale.horz.primary.text.shadow.dilate);
	setPrivateProfileReal(fileName, L"Design", L"scale.horz.primary.text.shadow.blur", scale.horz.primary.text.shadow.blur);
	setPrivateProfileInt(fileName, L"Design", L"scale.horz.primary.text.shadow.offset.x", scale.horz.primary.text.shadow.offset.x);
	setPrivateProfileInt(fileName, L"Design", L"scale.horz.primary.text.shadow.offset.y", scale.horz.primary.text.shadow.offset.y);
	setPrivateProfileColor(fileName, L"Design", L"scale.horz.primary.text.shadow.color", scale.horz.primary.text.shadow.color);
	setPrivateProfileInt(fileName, L"Design", L"scale.horz.secondary.height", scale.horz.secondary.height);
	setPrivateProfileLabel(fileName, L"Design", L"scale.horz.secondary.stroke.style", scale.horz.secondary.stroke.style, LineStyle::labels);
	setPrivateProfileInt(fileName, L"Design", L"scale.horz.secondary.stroke.width", scale.horz.secondary.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"scale.horz.secondary.stroke.color", scale.horz.secondary.stroke.color);

	setPrivateProfileInt(fileName, L"Design", L"scale.vert.width", scale.vert.width);
	setPrivateProfileInt(fileName, L"Design", L"scale.vert.text.height", scale.vert.text.height);
	setPrivateProfileColor(fileName, L"Design", L"scale.vert.text.color", scale.vert.text.color);
	setPrivateProfileReal(fileName, L"Design", L"scale.vert.text.shadow.dilate", scale.vert.text.shadow.dilate);
	setPrivateProfileReal(fileName, L"Design", L"scale.vert.text.shadow.blur", scale.vert.text.shadow.blur);
	setPrivateProfileInt(fileName, L"Design", L"scale.vert.text.shadow.offset.x", scale.vert.text.shadow.offset.x);
	setPrivateProfileInt(fileName, L"Design", L"scale.vert.text.shadow.offset.y", scale.vert.text.shadow.offset.y);
	setPrivateProfileColor(fileName, L"Design", L"scale.vert.text.shadow.color", scale.vert.text.shadow.color);
	setPrivateProfileLabel(fileName, L"Design", L"scale.vert.stroke.style", scale.vert.stroke.style, LineStyle::labels);
	setPrivateProfileInt(fileName, L"Design", L"scale.vert.stroke.width", scale.vert.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"scale.vert.stroke.color", scale.vert.stroke.color);
	setPrivateProfileLabel(fileName, L"Design", L"scale.vert.base.stroke.style", scale.vert.base.stroke.style, LineStyle::labels);
	setPrivateProfileInt(fileName, L"Design", L"scale.vert.base.stroke.width", scale.vert.base.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"scale.vert.base.stroke.color", scale.vert.base.stroke.color);

	setPrivateProfileColor(fileName, L"Design", L"graph.fill.color1", graph.fill.color1);
	setPrivateProfileColor(fileName, L"Design", L"graph.fill.color2", graph.fill.color2);
	setPrivateProfileLabel(fileName, L"Design", L"graph.stroke.style", graph.stroke.style, LineStyle::labels);
	setPrivateProfileInt(fileName, L"Design", L"graph.stroke.width", graph.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"graph.stroke.color", graph.stroke.color);
	setPrivateProfileLabel(fileName, L"Design", L"graph.current.stroke.style", graph.current.stroke.style, LineStyle::labels);
	setPrivateProfileInt(fileName, L"Design", L"graph.current.stroke.width", graph.current.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"graph.current.stroke.color", graph.current.stroke.color);
	setPrivateProfileLabel(fileName, L"Design", L"graph.hot.stroke.style", graph.hot.stroke.style, LineStyle::labels);
	setPrivateProfileInt(fileName, L"Design", L"graph.hot.stroke.width", graph.hot.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"graph.hot.stroke.color", graph.hot.stroke.color);
	setPrivateProfileLabel(fileName, L"Design", L"graph.last.stroke.style", graph.last.stroke.style, LineStyle::labels);
	setPrivateProfileInt(fileName, L"Design", L"graph.last.stroke.width", graph.last.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"graph.last.stroke.color", graph.last.stroke.color);

	setPrivateProfileLabel(fileName, L"Design", L"bpm.primary.stroke.style", bpm.primary.stroke.style, LineStyle::labels);
	setPrivateProfileInt(fileName, L"Design", L"bpm.primary.stroke.width", bpm.primary.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"bpm.primary.stroke.color", bpm.primary.stroke.color);
	setPrivateProfileInt(fileName, L"Design", L"bpm.primary.text.height", bpm.primary.text.height);
	setPrivateProfileColor(fileName, L"Design", L"bpm.primary.text.color", bpm.primary.text.color);
	setPrivateProfileReal(fileName, L"Design", L"bpm.primary.text.shadow.dilate", bpm.primary.text.shadow.dilate);
	setPrivateProfileReal(fileName, L"Design", L"bpm.primary.text.shadow.blur", bpm.primary.text.shadow.blur);
	setPrivateProfileInt(fileName, L"Design", L"bpm.primary.text.shadow.offset.x", bpm.primary.text.shadow.offset.x);
	setPrivateProfileInt(fileName, L"Design", L"bpm.primary.text.shadow.offset.y", bpm.primary.text.shadow.offset.y);
	setPrivateProfileColor(fileName, L"Design", L"bpm.primary.text.shadow.color", bpm.primary.text.shadow.color);
	setPrivateProfileLabel(fileName, L"Design", L"bpm.secondary.stroke.style", bpm.secondary.stroke.style, LineStyle::labels);
	setPrivateProfileInt(fileName, L"Design", L"bpm.secondary.stroke.width", bpm.secondary.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"bpm.secondary.stroke.color", bpm.secondary.stroke.color);
}

//--------------------------------------------------------------------
