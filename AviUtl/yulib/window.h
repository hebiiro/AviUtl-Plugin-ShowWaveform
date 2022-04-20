////////////////////////////////////////////////////////////////////////////////
// yulib/window.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef YULIB_WINDOW_H
#define YULIB_WINDOW_H

#include <stdio.h>
#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

namespace yulib {\

void SetClientSize(HWND hwnd, int width, int height)
{
	RECT wr, cr;

	GetWindowRect(hwnd, &wr);
	GetClientRect(hwnd, &cr);

	int new_width  = (wr.right - wr.left) - cr.right + width;
	int new_height = (wr.bottom - wr.top) - cr.bottom + height;
	SetWindowPos(hwnd, NULL, 0, 0, new_width, new_height, SWP_NOMOVE | SWP_NOZORDER);
}

class CWindow {
public:
	HWND hwnd;

	CWindow() : hwnd(NULL) {}
	CWindow(HWND hwnd) : hwnd(hwnd) {}

	operator HWND() { return hwnd; }
	HWND& operator=(const HWND& hwnd) { this->hwnd = hwnd; return this->hwnd; }

	void Attach(HWND hwnd)
	{
		this->hwnd = hwnd;
	}

	bool Create(LPCSTR classname, LPCSTR caption, DWORD style, DWORD exstyle, int x, int y, int width, int height, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param)
	{
		hwnd = CreateWindowEx(exstyle, classname, caption, style, x, y, width, height, parent, menu, inst, param);
		return hwnd != NULL;
	}

	BOOL Destroy(void)
	{
		return DestroyWindow(hwnd);
	}

	HWND GetChild(int id)
	{
		return GetDlgItem(hwnd, id);
	}

	BOOL Move(int x, int y, int w, int h, BOOL repaint)
	{
		return MoveWindow(hwnd, x, y, w, h, repaint);
	}

	void SetClientSize(int width, int height)
	{
		yulib::SetClientSize(hwnd, width, height);
	}

	int GetText(LPSTR buf, DWORD size)
	{
		return GetWindowText(hwnd, buf, size);
	}

	int GetTextLength(void)
	{
		return GetWindowTextLength(hwnd);
	}

	void SetRedraw(BOOL flag)
	{
		SendMessage(hwnd, WM_SETREDRAW, flag, 0);
	}

	void SetFont(HFONT font)
	{
		SendMessage(hwnd, WM_SETFONT, (WPARAM)font, 0);
	}

	LONG AddStyle(LONG style)
	{
		return SetWindowLongPtr(hwnd, GWL_STYLE, style | GetWindowLongPtr(hwnd, GWL_STYLE));
	}
};

class CListBox : public CWindow {
public:

	bool Create(DWORD style, DWORD exstyle, int x, int y, int w, int h, HWND parent, int id, HINSTANCE instance)
	{
		hwnd = CreateWindowEx(exstyle, "listbox", NULL, style, x, y, w, h, parent, (HMENU)id, instance, NULL);
		return hwnd != NULL;
	}


	int GetCurSel(void)
	{
		return (int)SendMessage(hwnd, LB_GETCURSEL, 0, 0);
	}

	void SetCurSel(int i)
	{
		SendMessage(hwnd, LB_SETCURSEL, i, 0);
	}

	void ResetContent(void)
	{
		SendMessage(hwnd, LB_RESETCONTENT, 0, 0);
	}
	int AddString(LPCSTR str)
	{
		return (int)SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)str);
	}
};



class CListView : public CWindow {
public:
	int InsertColumn(int i, LV_COLUMN *col)
	{
		// 新しいカラムの位置のインデックスを返す
		return ListView_InsertColumn(hwnd, i, col);
	}

	void DeleteAllItems(void)
	{
		ListView_DeleteAllItems(hwnd);
	}
};

} // namespace yu

#endif // #ifndef YULIB_CONTROL_H