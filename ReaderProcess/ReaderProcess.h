#pragma once

#include "Plugin.h"

//--------------------------------------------------------------------

#pragma pack(1)
struct int24_t {
	BYTE n[3];
	operator int32_t() const {
		struct {
			int32_t n:24;
		} s = { *(int32_t*)n };
		return s.n;
	}
};
#pragma pack()

//--------------------------------------------------------------------

struct ReaderProcess
{
	HINSTANCE m_instance;
	HWND m_client;
	Event m_event;
	SimpleFileMappingT<ReaderBottle> m_shared;
	Input::PluginPtr m_plugin;
	Input::MediaPtr m_media;

	BOOL init(HINSTANCE instance);
	BOOL term();

	BOOL initPlugin();
	BOOL termPlugin();

	BOOL receive();
	BOOL send();

	static float normalize(int8_t pcm);
	static float normalize(int16_t pcm);
	static float normalize(int24_t pcm);
	static float normalize(int32_t pcm);
	static float normalize(float pcm);
	template<typename T>
	static float calc(const T* samples, int count);
};

//--------------------------------------------------------------------
