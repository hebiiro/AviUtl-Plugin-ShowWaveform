////////////////////////////////////////////////////////////////////////////////
// yulib/path.h
// ÉpÉXä÷òAèàóù
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef YULIB_PATH_H
#define YULIB_PATH_H

#include <yulib/generic.h>
#include <yulib/string.h>

namespace yulib {\

inline bool PathIsDirectory(LPCSTR path)
{
	DWORD attr = GetFileAttributes(path);
	return (attr != ~0) && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

inline LPCSTR PathFindTitle(LPCSTR path)
{
	LPCSTR p = NULL;
	while(*path) {
		if(CharIsPathSeparator(*path)) { p = NULL; path++; }
		else {
			if(!p) p = path;
			if(CharIs2ByteLead(*path)) path += 2;
			else path++;
		}
	}

	return p;
}

inline void PathRemoveTitle(LPSTR path)
{
	LPSTR p = const_cast<LPSTR>(PathFindTitle(path));
	if(p && p > path) p[-1] = '\0';
}


class CPath {
public:
	char buf[MAX_PATH];

	CPath() { *buf = '\0'; }

	operator char*() { return buf; }
	LPSTR operator=(const HWND &hwnd)
	{
		GetWindowText(hwnd, buf, sizeof(buf));
		return buf;
	}

	DWORD Set(LPCSTR str) { return StringSet(buf, sizeof(buf), str); }
	void Clear(void) { *buf = '\0'; }

	BOOL OpenFileDialog(HWND owner, LPCSTR filter, LPCSTR caption)
	{
		OPENFILENAME ofn = {};
		ofn.lStructSize     = sizeof(ofn);
		ofn.hwndOwner       = owner;
		ofn.lpstrFilter     = filter;
		ofn.lpstrFile       = buf;
		ofn.nMaxFile        = sizeof(buf) / sizeof(*buf);
		//ofn.lpstrInitialDir = NULL;
		ofn.lpstrTitle      = caption;
		ofn.Flags           = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		return GetOpenFileName(&ofn);
	}

	BOOL SaveFileDialog(HWND owner, LPCSTR filter, LPCSTR caption)
	{
		OPENFILENAME ofn = {};
		ofn.lStructSize     = sizeof(ofn);
		ofn.hwndOwner       = owner;
		ofn.lpstrFilter     = filter;
		ofn.lpstrFile       = buf;
		ofn.nMaxFile        = sizeof(buf) / sizeof(*buf);
		//ofn.lpstrInitialDir = NULL;
		ofn.lpstrTitle      = caption;
		ofn.Flags           = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt     = "";
		return GetSaveFileName(&ofn);
	}
};

} // namespace yulib

#endif // #ifndef YULIB_PATH_H
