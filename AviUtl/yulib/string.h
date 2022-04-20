////////////////////////////////////////////////////////////////////////////////
// yulib/string.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef YULIB_STRING_H
#define YULIB_STRING_H

namespace yulib {\

inline bool CharIsUpper(char c) { return c >= 'A' && c <= 'Z'; }
inline bool CharIsLower(char c) { return c >= 'a' && c <= 'z'; }
inline bool CharIsAlpha(char c) { return CharIsUpper(c) || CharIsLower(c); }
inline bool CharIsNumeric(char c) { return c >= '0' && c <= '9'; }
inline bool CharIs1ByteKana(char c) { return c >= (char)0xA0 && c <= (char)0xDF; }
inline bool CharIs2ByteLead(char c) { return c >= (char)0x81 && c <= (char)0x9F || c >= (char)0xE0 && c <= (char)0xFC; }
inline bool CharIs2ByteTrail(char c) { return c >= (char)0x40 && c <= (char)0x7E || c >= (char)0x80 && c <= (char)0xFC; }
inline bool CharIsPathSeparator(char c) { return c == '\\' || c == '/'; }
inline bool CharIsInvalid(char c) { return c == (char)0x7F || c >= (char)0xFC && c <= (char)0xFF; }

inline char CharToUpper(char c) { return CharIsLower(c) ? c & ~0x20 : c; }
inline char CharToLower(char c) { return CharIsUpper(c) ? c | 0x20 : c; }

inline bool CharIsSame(char c1, char c2) { return c1 == c2; }
inline bool CharIsSame_i(char c1, char c2) { return CharToUpper(c1) == CharToUpper(c2); }

inline DWORD StringSet(LPSTR buf, DWORD size, LPCSTR str)
{
	LPSTR p = buf, end = buf + size-1;
	while(*str && p < end) *p++ = *str++;
	*p = '\0';
	return (DWORD)(p - buf);
}

} // namespace yulib

#endif // #ifndef YULIB_STRING_H