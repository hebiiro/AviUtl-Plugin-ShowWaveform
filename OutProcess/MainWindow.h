#pragma once

#include "OutProcess.h"
#include "Plugin.h"

//--------------------------------------------------------------------

typedef std::shared_ptr<Bottle> BottlePtr;

struct Cache
{
	std::string fileName;
	std::vector<Sample> samples;
};

typedef std::shared_ptr<Cache> CachePtr;
typedef std::map<std::string, CachePtr> CacheMap;

//--------------------------------------------------------------------

struct MainWindow
{
	HWND m_hwnd = 0;
	std::unique_ptr<Mutex> m_mutex;
	std::unique_ptr<SimpleFileMapping> m_fileMapping;
	CacheMap cacheMap;
	Input::PluginPtr plugin;

	BottlePtr getBottle();
	CachePtr getCache(const BottlePtr& bottle);
	CachePtr createCache(const BottlePtr& bottle);
	void getSample(const CachePtr& cache);
	void fireReceive(const CachePtr& cache);

	static float normalize(short pcm);
	static float calc(const std::vector<short>& samples);

	LRESULT onCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onDestroy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterExit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterSend(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterClear(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

};

 extern MainWindow g_mainWindow;

//--------------------------------------------------------------------