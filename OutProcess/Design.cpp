#include "pch.h"
#include "Design.h"

//--------------------------------------------------------------------

Design g_design;

//--------------------------------------------------------------------

const Design::Label Design::AlignX::labels[] = {
	{ left, L"left" },
	{ right, L"right" },
	{ center, L"center" },
};

const Design::Label Design::AlignY::labels[] = {
	{ top, L"top" },
	{ bottom, L"bottom" },
	{ center, L"center" },
};

const Design::Label Design::ScaleMode::labels[] = {
	{ normal, L"normal" },
	{ fit, L"fit" },
	{ crop, L"crop" },
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
	body.margin = 40;
	body.stroke.width = 1;
	body.stroke.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.5f);
	scale.width = 10;
	scale.text.height = 16;
	scale.text.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.8f);
	scale.text.shadow.blur = 2.0f;
	scale.text.shadow.offset = { 0, 1 };
	scale.text.shadow.color = nvgRGBAf(0.0f, 0.0f, 0.0f, 1.0f);
	scale.stroke.width = 1;
	scale.stroke.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.5f);
	scale.base.stroke.width = 1;
	scale.base.stroke.color = nvgRGBAf(1.0f, 0.0f, 0.0f, 0.5f);
	graph.fill.color1 = nvgRGBAf(1.0f, 1.0f, 0.0f, 0.4f);
	graph.fill.color2 = nvgRGBAf(1.0f, 1.0f, 0.0f, 0.4f);
	graph.stroke.width = 1;
	graph.stroke.color = nvgRGBAf(1.0f, 1.0f, 0.0f, 0.2f);
	graph.current.stroke.width = 1;
	graph.current.stroke.color = nvgRGBAf(0.0f, 1.0f, 0.0f, 0.4f);
	graph.current.text.height = 32;
	graph.current.text.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.8f);
	graph.current.text.shadow.blur = 4.0f;
	graph.current.text.shadow.offset = { 0, 1 };
	graph.current.text.shadow.color = nvgRGBAf(0.0f, 1.0f, 0.0f, 0.5f);
	graph.hot.stroke.width = 1;
	graph.hot.stroke.color = nvgRGBAf(1.0f, 0.0f, 1.0f, 0.4f);
	graph.hot.text.height = 32;
	graph.hot.text.color = nvgRGBAf(1.0f, 1.0f, 1.0f, 0.8f);
	graph.hot.text.shadow.blur = 4.0f;
	graph.hot.text.shadow.offset = { 0, 1 };
	graph.hot.text.shadow.color = nvgRGBAf(1.0f, 0.0f, 1.0f, 0.5f);
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

	getPrivateProfileInt(fileName, L"Design", L"body.margin", body.margin);
	getPrivateProfileInt(fileName, L"Design", L"body.stroke.width", body.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"body.stroke.color", body.stroke.color);

	getPrivateProfileInt(fileName, L"Design", L"scale.width", scale.width);
	getPrivateProfileInt(fileName, L"Design", L"scale.text.height", scale.text.height);
	getPrivateProfileColor(fileName, L"Design", L"scale.text.color", scale.text.color);
	getPrivateProfileReal(fileName, L"Design", L"scale.text.shadow.blur", scale.text.shadow.blur);
	getPrivateProfileInt(fileName, L"Design", L"scale.text.shadow.offset.x", scale.text.shadow.offset.x);
	getPrivateProfileInt(fileName, L"Design", L"scale.text.shadow.offset.y", scale.text.shadow.offset.y);
	getPrivateProfileColor(fileName, L"Design", L"scale.text.shadow.color", scale.text.shadow.color);
	getPrivateProfileInt(fileName, L"Design", L"scale.stroke.width", scale.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"scale.stroke.color", scale.stroke.color);
	getPrivateProfileInt(fileName, L"Design", L"scale.base.stroke.width", scale.base.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"scale.base.stroke.color", scale.base.stroke.color);

	getPrivateProfileColor(fileName, L"Design", L"graph.fill.color1", graph.fill.color1);
	getPrivateProfileColor(fileName, L"Design", L"graph.fill.color2", graph.fill.color2);
	getPrivateProfileInt(fileName, L"Design", L"graph.stroke.width", graph.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"graph.stroke.color", graph.stroke.color);
	getPrivateProfileInt(fileName, L"Design", L"graph.current.stroke.width", graph.current.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"graph.current.stroke.color", graph.current.stroke.color);
	getPrivateProfileInt(fileName, L"Design", L"graph.current.text.height", graph.current.text.height);
	getPrivateProfileColor(fileName, L"Design", L"graph.current.text.color", graph.current.text.color);
	getPrivateProfileReal(fileName, L"Design", L"graph.current.text.shadow.blur", graph.current.text.shadow.blur);
	getPrivateProfileInt(fileName, L"Design", L"graph.current.text.shadow.offset.x", graph.current.text.shadow.offset.x);
	getPrivateProfileInt(fileName, L"Design", L"graph.current.text.shadow.offset.y", graph.current.text.shadow.offset.y);
	getPrivateProfileColor(fileName, L"Design", L"graph.current.text.shadow.color", graph.current.text.shadow.color);
	getPrivateProfileInt(fileName, L"Design", L"graph.hot.stroke.width", graph.hot.stroke.width);
	getPrivateProfileColor(fileName, L"Design", L"graph.hot.stroke.color", graph.hot.stroke.color);
	getPrivateProfileInt(fileName, L"Design", L"graph.hot.text.height", graph.hot.text.height);
	getPrivateProfileColor(fileName, L"Design", L"graph.hot.text.color", graph.hot.text.color);
	getPrivateProfileReal(fileName, L"Design", L"graph.hot.text.shadow.blur", graph.hot.text.shadow.blur);
	getPrivateProfileInt(fileName, L"Design", L"graph.hot.text.shadow.offset.x", graph.hot.text.shadow.offset.x);
	getPrivateProfileInt(fileName, L"Design", L"graph.hot.text.shadow.offset.y", graph.hot.text.shadow.offset.y);
	getPrivateProfileColor(fileName, L"Design", L"graph.hot.text.shadow.color", graph.hot.text.shadow.color);
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

	setPrivateProfileInt(fileName, L"Design", L"body.margin", body.margin);
	setPrivateProfileInt(fileName, L"Design", L"body.stroke.width", body.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"body.stroke.color", body.stroke.color);

	setPrivateProfileInt(fileName, L"Design", L"scale.width", scale.width);
	setPrivateProfileInt(fileName, L"Design", L"scale.text.height", scale.text.height);
	setPrivateProfileColor(fileName, L"Design", L"scale.text.color", scale.text.color);
	setPrivateProfileReal(fileName, L"Design", L"scale.text.shadow.blur", scale.text.shadow.blur);
	setPrivateProfileInt(fileName, L"Design", L"scale.text.shadow.offset.x", scale.text.shadow.offset.x);
	setPrivateProfileInt(fileName, L"Design", L"scale.text.shadow.offset.y", scale.text.shadow.offset.y);
	setPrivateProfileColor(fileName, L"Design", L"scale.text.shadow.color", scale.text.shadow.color);
	setPrivateProfileInt(fileName, L"Design", L"scale.stroke.width", scale.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"scale.stroke.color", scale.stroke.color);
	setPrivateProfileInt(fileName, L"Design", L"scale.base.stroke.width", scale.base.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"scale.base.stroke.color", scale.base.stroke.color);

	setPrivateProfileColor(fileName, L"Design", L"graph.fill.color1", graph.fill.color1);
	setPrivateProfileColor(fileName, L"Design", L"graph.fill.color2", graph.fill.color2);
	setPrivateProfileInt(fileName, L"Design", L"graph.stroke.width", graph.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"graph.stroke.color", graph.stroke.color);
	setPrivateProfileInt(fileName, L"Design", L"graph.current.stroke.width", graph.current.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"graph.current.stroke.color", graph.current.stroke.color);
	setPrivateProfileInt(fileName, L"Design", L"graph.current.text.height", graph.current.text.height);
	setPrivateProfileColor(fileName, L"Design", L"graph.current.text.color", graph.current.text.color);
	setPrivateProfileReal(fileName, L"Design", L"graph.current.text.shadow.blur", graph.current.text.shadow.blur);
	setPrivateProfileInt(fileName, L"Design", L"graph.current.text.shadow.offset.x", graph.current.text.shadow.offset.x);
	setPrivateProfileInt(fileName, L"Design", L"graph.current.text.shadow.offset.y", graph.current.text.shadow.offset.y);
	setPrivateProfileColor(fileName, L"Design", L"graph.current.text.shadow.color", graph.current.text.shadow.color);
	setPrivateProfileInt(fileName, L"Design", L"graph.hot.stroke.width", graph.hot.stroke.width);
	setPrivateProfileColor(fileName, L"Design", L"graph.hot.stroke.color", graph.hot.stroke.color);
	setPrivateProfileInt(fileName, L"Design", L"graph.hot.text.height", graph.hot.text.height);
	setPrivateProfileColor(fileName, L"Design", L"graph.hot.text.color", graph.hot.text.color);
	setPrivateProfileReal(fileName, L"Design", L"graph.hot.text.shadow.blur", graph.hot.text.shadow.blur);
	setPrivateProfileInt(fileName, L"Design", L"graph.hot.text.shadow.offset.x", graph.hot.text.shadow.offset.x);
	setPrivateProfileInt(fileName, L"Design", L"graph.hot.text.shadow.offset.y", graph.hot.text.shadow.offset.y);
	setPrivateProfileColor(fileName, L"Design", L"graph.hot.text.shadow.color", graph.hot.text.shadow.color);
}

//--------------------------------------------------------------------
