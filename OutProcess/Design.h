#pragma once

#include "OutProcess.h"

//--------------------------------------------------------------------

struct XYWHRect
{
	int x, y, w, h;

	XYWHRect()
	{
		x = y = w = h = 0;
	}

	XYWHRect(int x, int y, int w, int h)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	XYWHRect(const RECT& rc)
	{
		operator=(rc);
	}

	XYWHRect& operator=(const RECT& rc)
	{
		this->x = rc.left;
		this->y = rc.top;
		this->w = getWidth(rc);
		this->h = getHeight(rc);

		return *this;
	}
};

struct MyColor : public NVGcolor
{
	MyColor()
	{
	}

	MyColor(DWORD color)
	{
		this->r = GetRValue(color) / 255.0f;
		this->g = GetGValue(color) / 255.0f;
		this->b = GetBValue(color) / 255.0f;
		this->a = GetAValue(color) / 255.0f;
	}

	operator DWORD()
	{
		BYTE r = (BYTE)roundf(this->r * 255.0f);
		BYTE g = (BYTE)roundf(this->g * 255.0f);
		BYTE b = (BYTE)roundf(this->b * 255.0f);
		BYTE a = (BYTE)roundf(this->a * 255.0f);

		return getDWORD(r, g, b, a);
	}

	MyColor& operator=(const NVGcolor& color)
	{
		((NVGcolor&)*this) = color;

		return *this;
	}

	static BYTE GetAValue(DWORD color)
	{
		return LOBYTE(color >> 24);
	}

	static COLORREF getCOLORREF(DWORD rgba)
	{
		return (rgba & 0x00FFFFFF);
	}

	static DWORD getDWORD(BYTE r, BYTE g, BYTE b, BYTE a)
	{
		return r | ((WORD)g << 8) | ((DWORD)b << 16) | ((DWORD)a << 24);
	}

	static DWORD getDWORD(COLORREF color, DWORD rgba)
	{
		return (color & 0x00FFFFFF) | (rgba & 0xFF000000);
	}
};

struct StringU8
{
	char m_buffer[MAX_PATH] = {};

	StringU8()
	{
	}

	StringU8(LPCWSTR x)
	{
		operator=(x);
	}

	StringU8& operator=(LPCWSTR x)
	{
		::WideCharToMultiByte(CP_UTF8, 0, x, -1, m_buffer, MAX_PATH, 0, 0);
		return *this;
	}

	operator LPCSTR() const
	{
		return m_buffer;
	}
};

//--------------------------------------------------------------------

struct Design
{
	struct AlignX {
		static const int left = 0;
		static const int right = 1;
		static const int center = 2;
		static const Label labels[];
	};

	struct AlignY {
		static const int top = 0;
		static const int bottom = 1;
		static const int center = 2;
		static const Label labels[];
	};

	struct ScaleMode {
		static const int normal = 0;
		static const int fit = 1;
		static const int crop = 2;
		static const Label labels[];
	};

	struct LineStyle {
		static const Label labels[];
	};

	struct Fill {
		MyColor color;
	};

	struct GradientFill {
		MyColor color1;
		MyColor color2;
	};

	struct Stroke {
		int style;
		int width;
		MyColor color;
	};

	struct Text {
		int height;
		MyColor color;
		struct Shadow {
			float dilate;
			float blur;
			POINT offset;
			MyColor color;
		} shadow;
	};

	struct Marker {
		Stroke stroke;
	};

	_bstr_t fontDefault;
	_bstr_t fontDefault2;
	struct Image {
		_bstr_t fileName;
		POINT offset;
		POINT align;
		int scaleMode;
		float scale;
		float angle;
		float alpha;
	} image;
	struct Background {
		GradientFill fill;
	} background;
	struct Body {
		struct Margin {
			int x;
			int y;
		} margin;
		Stroke stroke;
	} body;
	struct Scale {
		struct Horz {
			int minUnitWidth;
			struct Primary {
				int height;
				Stroke stroke;
				Text text;
			} primary;
			struct Secondary {
				int height;
				Stroke stroke;
			} secondary;
		} horz;
		struct Vert {
			int width;
			Text text;
			Stroke stroke;
			struct Base {
				Stroke stroke;
			} base;
		} vert;
	} scale;
	struct BPM {
		struct Primary {
			Stroke stroke;
			Text text;
		} primary;
		struct Secondary {
			Stroke stroke;
		} secondary;
	} bpm;
	struct Graph {
		GradientFill fill;
		Stroke stroke;
		Marker current;
		Marker hot;
		Marker last;
	} graph;

	Design();
	~Design();

	void load(LPCWSTR fileName);
	void save(LPCWSTR fileName);
};

extern Design g_design;

//--------------------------------------------------------------------
