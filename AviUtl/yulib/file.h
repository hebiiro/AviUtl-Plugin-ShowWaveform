////////////////////////////////////////////////////////////////////////////////
// yulib/file.h
// ファイル関連処理
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef YULIB_FILE_H
#define YULIB_FILE_H

#include <yulib/generic.h>

namespace yulib {\

inline bool OutputFile(LPCSTR filename, LPCVOID data, DWORD size)
{
	HANDLE file = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(file == INVALID_HANDLE_VALUE) return false;
	WriteFile(file, data, size, &size, NULL);
	CloseHandle(file);
	return true;
}

class CFile {
public:
	HANDLE file;

	CFile() : file(INVALID_HANDLE_VALUE) {}
	~CFile() { if(*this) CloseHandle(file); }

	operator HANDLE() { return file; }
	operator bool() { return file != INVALID_HANDLE_VALUE; }

	bool OpenExisting(LPCSTR filename)
	{
		if(!Close()) return false;
		file = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		return file != INVALID_HANDLE_VALUE;
	}

	bool CreateAlways(LPCSTR filename)
	{
		if(!Close()) return false;
		file = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		return file != INVALID_HANDLE_VALUE;
	}

	bool Close(void)
	{
		if(file == INVALID_HANDLE_VALUE) return true;
		if(!CloseHandle(file)) return false;
		file = INVALID_HANDLE_VALUE;
		return true;
	}

	DWORD Size(void)
	{
		return GetFileSize(file, NULL);
	}

	DWORD Remaining(void)
	{
		return SetFilePointer(file, 0, NULL, FILE_CURRENT);
	}

	BOOL Read(LPVOID buf, DWORD size)
	{
		return ReadFile(file, buf, size, &size, NULL);
	}

	BOOL Write(LPCVOID data, DWORD size)
	{
		return WriteFile(file, data, size, &size, NULL);
	}
};

} // namespace yulib

#endif // #ifndef YULIB_FILE_H