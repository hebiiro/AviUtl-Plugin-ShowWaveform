////////////////////////////////////////////////////////////////////////////////
// yulib/memory.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef YULIB_MEMORY_H
#define YULIB_MEMORY_H

#include <yulib/generic.h>

namespace yulib {\


inline void* MemAlloc(DWORD size, bool zeroinit)
{
	return HeapAlloc(GetProcessHeap(), zeroinit ? HEAP_ZERO_MEMORY : 0, size);
}

inline void* MemReAlloc(void *mem, DWORD size, bool zeroinit)
{
	return HeapReAlloc(GetProcessHeap(), zeroinit ? HEAP_ZERO_MEMORY : 0, mem, size);
}

inline BOOL MemFree(void *mem)
{
	return HeapFree(GetProcessHeap(), 0, mem);
}

inline DWORD MemSize(void *mem)
{
	return HeapSize(GetProcessHeap(), 0, mem);
}

/*
class CBinaryMemory {
public:
	LPBYTE mem;
	DWORD  size;

	CBinaryMemory() : mem(NULL), size(0) {}
	~CBinaryMemory() { if(mem) MemFree(mem); }

	operator LPBYTE() { return mem; }
	operator bool() { return mem != NULL; }

	bool Alloc(DWORD size, bool zeroinit)
	{
		if(mem) {
			LPBYTE temp = 
		} else {
			mem = MemAlloc(size, zeroinit);
			if(!mem) return false;
			this->size = size;
			return true;
		}
	}
};*/

template <typename T>
class CMemory {
public:
	T* mem;
	DWORD size;

	CMemory() : mem(NULL), size(0) {}
	~CMemory() { if(mem) MemFree(mem); }

	operator T*() { return mem; }
	operator bool() { return mem != NULL; }
	
	template <typename U>
	T* operator+ (U v) { return mem + v; }

	template <typename U>
	T& operator[] (U v) { return mem[v]; }


	bool Alloc(DWORD size, bool zeroinit)
	{
		if(!mem) {
			mem = (T*)MemAlloc(size * sizeof(T), zeroinit);
			if(!mem) return false;
			this->size = size;
			return true;
		} else {
			T* temp = (T*)MemReAlloc(mem, size * sizeof(T), zeroinit);
			if(!temp) return false;
			mem = temp;
			this->size = size;
			return true;
		}
	}

	bool Free(void)
	{
		if(!mem) return true;
		free(mem);
		mem = NULL;
		size = 0;
		return true;
	}
};

template <typename T, size_t N>
class CStaticArray {
public:
	static const size_t max_size = N;

	T buf[N];
	size_t size;

	CStaticArray() : size(0) {}

	template <typename U>
	T& operator[] (U i) { return buf[i]; }

	bool IsMax(void)
	{
		return size == max_size;
	}

	void Clear(void)
	{
		size = 0;
	}

	void Insert(const T& v, size_t idx)
	{
		int max = size >= max_size ? max_size-1 : size;
		for(int i = max; i > (int)idx;) {
			i--;
			buf[i+1] = buf[i];
		}
		buf[idx] = v;
		if(size < max_size) size++;
	}

	void Erase(int idx)
	{
		size--;
		for(int i = idx; i < (int)size; i++) {
			buf[i] = buf[i+1];
		}
	}
};

template <typename T>
class CVector {
public:
	T* mem;
	size_t size;
	size_t capacity;

	CVector() : mem(NULL), size(0), capacity(0) {}
	~CVector() { free(mem); }

	bool Alloc(size_t size)
	{
		if(!mem) {
			mem = (T*)malloc(size * sizeof(T));
			if(!mem) return false;
			this->capacity = size;
			return true;
		} else {
			T* temp = (T*)realloc(mem, size * sizeof(T));
			if(!temp) return false;
			mem = temp;
			capacity = size;
			return true;
		}
	}

	void Free(void)
	{
		free(mem);
		size = 0;
		capacity = 0;
	}
};

class CTextBuffer {
public:
	LPSTR mem;
	LPSTR ptr;
	LPSTR end;
	DWORD size;

	CTextBuffer() : mem(NULL), ptr(NULL), end(NULL), size(0) {}

	~CTextBuffer() { if(mem) free(mem); }

	operator LPSTR() { return mem; }

	CTextBuffer& operator << (LPCSTR str) { Write(str); return *this; }

	DWORD StrLen(void) { return (DWORD)(ptr - mem); }

	bool Alloc(DWORD size)
	{
		if(!mem) {
			mem = (LPSTR)malloc(size);
			if(!mem) return false;
			ptr = mem;
			end = mem + size;
			this->size = size;
			return true;
		} else {
			DWORD offset = (DWORD)(ptr - mem);
			LPSTR temp = (LPSTR)realloc(mem, size);
			if(!temp) return false;
			mem = temp;
			ptr = temp + offset;
			end = temp + size;
			this->size = size;
			return true;
		}
	}

	bool Extend(DWORD size)
	{
		return Alloc(this->size + size);
	}

	bool Write(LPCSTR str, DWORD size)
	{
		DWORD rest = (DWORD)(end - ptr);
		if(rest <= size && !Extend(this->size * 2 < size ? size : this->size * 2)) return false;
		while(*str) *ptr++ = *str++;
		return true;
	}

	bool Write(LPCSTR str)
	{
		return Write(str, strlen(str));
	}

	bool WriteF(LPCSTR fmt, ...)
	{
		char buf[256];
		va_list args;
		va_start(args, fmt);
		DWORD len = (DWORD)vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
		va_end(args);
		return Write(buf, len);
	}
};

} // namespace yulib

#endif // #ifndef YULIB_MEMORY_H
