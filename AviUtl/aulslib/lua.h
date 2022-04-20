////////////////////////////////////////////////////////////////////////////////
// aulslib/lua.h
// LuaDLL用ヘッダ
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef AULSLIB_LUA_H
#define AULSLIB_LUA_H

#include <yulib/color.h>
#include <lua.hpp>

#pragma comment(lib, "lua51.lib")

namespace auls {\

using yulib::ColorRGBA;

inline DWORD luaA_todword(lua_State *L, int index)
{
	return (DWORD)lua_tonumber(L, index);
}

// index が負の値(相対指定)の時、正の値(絶対指定)に変換する
inline int luaA_positivateindex(lua_State *L, int index)
{
	if(index < 0 && index > LUA_REGISTRYINDEX) return lua_gettop(L) + index + 1;
	else return index;
}

// index が指すテーブル内に、名前が <name> のテーブルを作成しスタックに積む
// すでに存在していればそれをスタックに積む
inline void luaA_newtable(lua_State *L, int index, LPCSTR name)
{
	index = luaA_positivateindex(L, index);

	lua_getfield(L, index, name);
	if(lua_type(L, -1) != LUA_TTABLE) {
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setfield(L, index, name);
	}
}

// グローバルテーブル内に <libname1> テーブルを作り、その中に <libname2> テーブルを作り関数を登録する
inline void luaA_register(lua_State *L, LPCSTR libname1, LPCSTR libname2, const luaL_reg *table)
{
	luaA_newtable(L, LUA_GLOBALSINDEX, libname1);
	luaA_newtable(L, -1, libname2);
	while(table->name && table->func) {
		lua_pushcfunction(L, table->func);
		lua_setfield(L, -2, table->name);
		table++;
	}
}

class luaA_PixelData {
public:
	yulib::ColorRGBA *data;
	int        width;
	int        height;

	luaA_PixelData(lua_State *L, int index)
	{
		data = (yulib::ColorRGBA*)lua_touserdata(L, index+0);
		if(!data) luaL_typerror(L, index, "userdata");
		width  = lua_tointeger(L, index+1);
		height = lua_tointeger(L, index+2);
	}

	template <typename T>
	yulib::ColorRGBA& operator[](T v) { return data[v]; }

	int Size(void)
	{
		return width * height;
	}

	void Put(int x, int y, yulib::ColorRGBA col)
	{
		data[x + y * width] = col;
	}
};

class luaA_PixelData2 {
public:
	yulib::ColorRGBA *data1;
	yulib::ColorRGBA *data2;
	int width;
	int height;

	luaA_PixelData2(lua_State *L, int index)
	{
		data1 = (ColorRGBA*)lua_touserdata(L, index);
		if(!data1) luaL_typerror(L, index, "userdata");
		data2 = (ColorRGBA*)lua_touserdata(L, index+1);
		if(!data2) luaL_typerror(L, index+1, "userdata");
		width = (int)lua_tointeger(L, index+2);
		height = (int)lua_tointeger(L, index+3);
	}

	template <typename T>
	ColorRGBA* operator[] (T i) { return ((ColorRGBA**)this)[i]; }

	int Size(void)
	{
		return width * height;
	}
};

template <typename T, int N>
class luaA_StaticMultiArgs {
public:
	static const int max_size = N;

	T v[N];

	template <typename U>
	luaA_StaticMultiArgs(lua_State *L, int idx, U f)
	{
		for(int i = 0; i < N; i++) v[i] = (T)f(L, idx+i);
	}

	template <typename U>
	T& operator[](U i) { return v[i]; }
};

template <typename T>
class luaA_TableArgs {
public:
	T* buf;
	int num;

	template <typename U>
	luaA_TableArgs(lua_State *L, int index, U f)
	{
		if(lua_type(L, index) != LUA_TTABLE) luaL_typerror(L, index, "table");
		num = lua_objlen(L, index);
		if(num == 0) return;
		buf = (T*)malloc(sizeof(T) * num);
		if(!buf) luaL_error(L, "auls::luaA_TableArgs : メモリの確保に失敗しました。");

		for(int i = 0; i < num; i++) {
			lua_pushinteger(L, i+1);
			lua_gettable(L, index);
			buf[i] = f(L, -1);
			lua_pop(L, 1);
		}
	}

	~luaA_TableArgs(void)
	{
		if(buf) free(buf);
	}

	template <typename U>
	T& operator[] (U i) { return buf[i]; }
};

#define AULSLIB_LUA_EXPORT(filename, lib1, lib2) extern "C" __declspec(dllexport) int luaopen_##filename(lua_State *L) { auls::luaA_register(L, lib1, lib2, 
} // namespace auls

#endif // #ifndef AULSLIB_LUA_H