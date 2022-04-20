////////////////////////////////////////////////////////////////////////////////
// yulib/color.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef YULIB_COLOR_H
#define YULIB_COLOR_H

#include <yulib/generic.h>

namespace yulib {\

struct ColorRGB;
struct ColorRGBA;
struct ColorHSV;
struct ColorYCbCr;
struct ColorYCbCrA;

struct ColorRGB {
public:
	struct { BYTE b, g, r; };

	ColorRGB() {}
	ColorRGB(DWORD v) : b((BYTE)v), g((BYTE)(v >> 8)), r((BYTE)(v >> 16)) {}
	ColorRGB(BYTE r, BYTE g, BYTE b) : b(b), g(g), r(r) {}
	ColorRGB(const ColorRGBA& rgba) { *this = rgba; }
	ColorRGB(const ColorHSV& hsv)   { *this = hsv; }

	operator DWORD() { return b | (g << 8) | (r << 16); }
	ColorRGB& operator=(const ColorRGBA&);
	ColorRGB& operator=(const ColorHSV&);

	ColorRGB Grad(ColorRGB col2, BYTE ratio)
	{
		ColorRGB temp;
		temp.r = (this->r * (255 - ratio) + col2.r * ratio) / 255;
		temp.g = (this->g * (255 - ratio) + col2.g * ratio) / 255;
		temp.b = (this->b * (255 - ratio) + col2.b * ratio) / 255;
		return temp;
	}
};

struct ColorRGBA {
public:
	union {
		DWORD val;
		struct { BYTE b, g, r, a; };
	};

	ColorRGBA() {}
	ColorRGBA(DWORD v) : val(v) {}

	ColorRGBA& operator=(const DWORD &v)
	{
		this->val = v;
		return *this;
	}

	ColorRGBA& operator=(const ColorRGB& rgb);
	ColorRGBA& operator=(const ColorHSV& hsv);

	ColorRGBA Grad(ColorRGBA col2, BYTE ratio)
	{
		ColorRGBA temp;
		temp.r = (this->r * (255 - ratio) + col2.r * ratio) / 255;
		temp.g = (this->g * (255 - ratio) + col2.g * ratio) / 255;
		temp.b = (this->b * (255 - ratio) + col2.b * ratio) / 255;
		temp.a = (this->a * (255 - ratio) + col2.a * ratio) / 255;
		return temp;
	}

	ColorRGBA Blend(ColorRGB col)
	{
		ColorRGBA temp;
		temp.r = (this->r * this->a + col.r * (255 - this->a)) / 255;
		temp.g = (this->g * this->a + col.g * (255 - this->a)) / 255;
		temp.b = (this->b * this->a + col.b * (255 - this->a)) / 255;
		temp.a = 255;
		return temp;
	}
};

struct ColorHSV {
public:
	WORD h; // 0 - 359
	BYTE s, v; // 0 - 255

	ColorHSV() {}
	ColorHSV(WORD h, BYTE s, BYTE v) : h(h), s(s), v(v) {}
	ColorHSV(const ColorRGB &rgb) { *this = rgb; }

	ColorHSV& operator=(const ColorRGB  &rgb);
	ColorHSV& operator=(const ColorRGBA &rgba);
};

struct ColorYCbCrA {
	short y;
	short cb;
	short cr;
	short a;
};

////////////////////////////////////////////////////////////////////////////////
// êFïœä∑ä÷êî
////////////////////////////////////////////////////////////////////////////////


// RGB <- RGBA
inline ColorRGB& ColorRGB::operator=(const ColorRGBA &rgba)
{
	this->r = rgba.r;
	this->g = rgba.g;
	this->b = rgba.b;
	return *this;
}

// RGB <- HSV
inline ColorRGB& ColorRGB::operator= (const ColorHSV &hsv)
{
	BYTE hi = hsv.h / 60 % 6;
	BYTE f = hsv.h % 60;
	BYTE p = hsv.v - hsv.s * hsv.v / 255;
	BYTE q = hsv.v - hsv.s * hsv.v * f / 255 / 60;
	BYTE t = hsv.v - hsv.s * hsv.v * (60-f) / 255 / 60;

	switch(hi) {
		case 0: this->r = hsv.v; this->g = t;     this->b = p; break;
		case 1: this->r = q;     this->g = hsv.v; this->b = p; break;
		case 2: this->r = p;     this->g = hsv.v; this->b = t; break;
		case 3: this->r = p;     this->g = q;     this->b = hsv.v; break;
		case 4: this->r = t;     this->g = p;     this->b = hsv.v; break;
		case 5: this->r = hsv.v; this->g = p;     this->b = q;     break;
	}

	return *this;
}

// RGBA <- RGB
inline ColorRGBA& ColorRGBA::operator= (const ColorRGB &rgb)
{
	this->r = rgb.r;
	this->g = rgb.g;
	this->b = rgb.b;
	return *this;
}

// RGBA <- HSV
inline ColorRGBA& ColorRGBA::operator= (const ColorHSV &hsv)
{
	ColorRGB rgb = hsv;
	*this = rgb;
	return *this;
}


// HSV <- RGB
inline ColorHSV& ColorHSV::operator= (const ColorRGB &rgb)
{
	BYTE max = yulib::Max(rgb.r, rgb.g, rgb.b);
	BYTE min = yulib::Min(rgb.r, rgb.g, rgb.b);

	if(max == min) this->h = 0;
	else if(max == rgb.r) this->h = (60 * (rgb.g - rgb.b) / (max - min) + 360) % 360;
	else if(max == rgb.g) this->h = 60 * (rgb.b - rgb.r) / (max - min) + 120;
	else if(max == rgb.b) this->h = 60 * (rgb.r - rgb.g) / (max - min) + 240;

	if(max == 0) this->s = 0;
	else this->s = 255 * (max - min) / max;

	this->v = max;

	return *this;
}

// HSV <- RGBA
inline ColorHSV& ColorHSV::operator=(const ColorRGBA &rgba)
{
	ColorRGB rgb = rgba;
	*this = rgb;
	return *this;
}

} // namespace yulib

#endif // #ifndef YULIB_COLOR_H