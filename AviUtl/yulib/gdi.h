////////////////////////////////////////////////////////////////////////////////
// yulib/gdi.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef YULIB_GDI_H
#define YULIB_GDI_H

#include <stdio.h>
#include <windows.h>

namespace yulib {\

class CBrush {
public:
	HBRUSH brush;

	CBrush() : brush(NULL) {}

	operator HBRUSH() { return brush; }

	bool CreateSolid(COLORREF color)
	{
		brush = CreateSolidBrush(color);
		return brush != NULL;
	}

	void Delete(void)
	{
		DeleteObject(brush);
		brush = NULL;
	}
};

class CPen {
public:
	HPEN pen;

	CPen() : pen(NULL) {}

	operator HPEN() { return pen; }

	bool Create(int style, int width, COLORREF color)
	{
		pen = CreatePen(style, width, color);
		return pen != NULL;
	}

	void Delete(void)
	{
		DeleteObject(pen);
		pen = NULL;
	}
};

} // namespace yu

#endif // #ifndef YULIB_GDI_H