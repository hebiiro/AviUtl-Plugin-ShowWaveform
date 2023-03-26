#pragma once

#include "Plugin.h"

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

	static float normalize(char pcm);
	static float normalize(short pcm);
	static float normalize(long pcm);
	static float normalize(float pcm);
	template<typename T>
	static float calc(const T* samples, int count);
	static float normalize24(long pcm);
	static long convert24(const BYTE* sample);
	static float calc24(const BYTE* samples, int count);
};

//--------------------------------------------------------------------
