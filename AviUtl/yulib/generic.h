////////////////////////////////////////////////////////////////////////////////
// yulib/generic.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef YULIB_GENERIC_H
#define YULIB_GENERIC_H

#include <stdio.h>
#include <windows.h>

#undef min
#undef max

namespace yulib {\

#define YULIB_EXPORT extern "C" __declspec(dllexport)

////////////////////////////////////////////////////////////////////////////////
// テンプレート関数
////////////////////////////////////////////////////////////////////////////////

template <typename T>
const T& Min(const T& a, const T& b) { return a <= b ? a : b; }

template <typename T>
const T& Min(const T& a, const T& b, const T& c)
{ return Min(a, Min(b, c)); }

template <typename T>
const T& Max(const T& a, const T& b) { return a >= b ? a : b; }

template <typename T>
const T& Max(const T& a, const T& b, const T& c)
{ return Max(a, Max(b, c)); }

template <typename T>
T Diff(T a, T b) { return a > b ? a - b : b - a; }

template <typename T>
void Swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}

inline void Set4ByteBE(LPBYTE p, DWORD v)
{
	LPCBYTE vp = (LPCBYTE)&v;
	p[0] = vp[3]; p[1] = vp[2]; p[2] = vp[1]; p[3] = vp[0];
}

inline DWORD Get4ByteBE(LPCBYTE p)
{
	return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]);
}

inline DWORD CheckSum(LPCVOID data, DWORD size)
{
	LPCBYTE p = (LPCBYTE)data, end = p + size;
	DWORD sum = 0;
	while(p < end) sum += *p++;
	return sum;
}

inline void MakeCrc32Table(DWORD *table)
{
	const DWORD CRCPOLY = 0xEDB88320UL;

	for(int i = 0; i < 256; i++) {
		DWORD v = (DWORD)i;
		for(int j = 0; j < 8; j++) {
			if(v & 1) v = (v >> 1) ^ CRCPOLY;
			else v >>= 1;
		}
		table[i] = v;
	}
}

inline DWORD Crc32(LPCVOID data, DWORD size, DWORD *table)
{
	LPCBYTE p = (LPCBYTE)data, end = p + size;
	DWORD crc = ~0;
	while(p < end) crc = (crc >> 8) ^ table[(BYTE)crc ^ *p++];
	return ~crc;
}

inline DWORD Crc32(LPCVOID data, DWORD size)
{
	DWORD table[256];
	MakeCrc32Table(table);
	return Crc32(data, size, table);
}

inline void DbgMsg(LPCSTR fmt, ...)
{
	char buf[256];
	va_list args;
	va_start(args, fmt);
	vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
	va_end(args);
	OutputDebugString(buf);
}

inline void GetLastErrorString(LPSTR buf, DWORD size)
{
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, size, NULL);
}

template <typename T, T min_, T max_>
class CStaticRangedValue {
public:
	static const T min = min_;
	static const T max = max_;

	T v;

	CStaticRangedValue() {}
	CStaticRangedValue(T v) : v(v) {}

	T operator++()    { v < max ? ++v : v; }
	T operator--()    { v > min ? --v : v; }
	T operator++(int) { v < max ? v++ : v; }
	T operator--(int) { v > min ? v-- : v; }
};


template <typename T>
class auto_cast_t {
public:
	T &v;

	auto_cast_t(T &v) : v(v) {}

	template <typename U>
	operator U() { return (U)v; }
};

template <typename T>
auto_cast_t<T> auto_cast(T v)
{
	return auto_cast_t<T>(v);
}

class format {
public:
	char buf[256];

	format(LPCSTR fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
		va_end(args);
	}

	operator LPSTR() { return buf; }
};

} // namespace auls

#endif // #ifndef YULIB_GENERIC_H