#include "pch.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

MainWindow g_mainWindow;

//--------------------------------------------------------------------

BottlePtr MainWindow::getBottle()
{
	BottlePtr bottle = std::make_shared<Bottle>();

	if (m_mutex && m_fileMapping)
	{
//		Synchronizer sync(*m_mutex);
		Bottle* shared = (Bottle*)m_fileMapping->getBuffer();

		memcpy(bottle.get(), shared, sizeof(*shared));
	}
	else
	{
		return 0;
	}

	return bottle;
}

CachePtr MainWindow::getCache(const BottlePtr& bottle)
{
	auto it = cacheMap.find(bottle->fileName);
	if (it == cacheMap.end()) return 0;
	return it->second;
}

CachePtr MainWindow::createCache(const BottlePtr& bottle)
{
	MY_TRACE(_T("MainWindow::createCache()\n"));

	CachePtr cache = getCache(bottle);
	if (cache) return cache;

	cache = std::make_shared<Cache>();
	cacheMap[bottle->fileName] = cache;

	cache->fileName = bottle->fileName;
	MY_TRACE_STR(cache->fileName.c_str());

	getSample(cache);

	return cache;
}

void MainWindow::getSample(const CachePtr& cache)
{
	MY_TRACE(_T("MainWindow::getSample()\n"));

	Input::MediaPtr media = std::make_shared<Input::Media>();
	if (!media->open(plugin, cache->fileName.c_str()))
		return;

	AviUtl::InputPluginDLL* ip = plugin->getInputPlugin();
	AviUtl::InputHandle handle = media->getInputHandle();
	MediaInfo* mi = media->getMediaInfo();

	int32_t start = 0;
	int32_t length = mi->audio_format.nSamplesPerSec / SAMPLE_FPS;
	int32_t bufferSize = mi->audio_format.nChannels * length;
	std::vector<short> buffer(bufferSize);

	MY_TRACE_INT(mi->audio_format.nSamplesPerSec);
	MY_TRACE_INT(length);
	MY_TRACE_INT(bufferSize);

	for (int i = 0; ; i++)
	{
		int32_t read = ip->func_read_audio(handle, start, length, buffer.data());

		start += read;

		Sample sample;
		sample.level = calc(buffer);
		cache->samples.emplace_back(sample);

		MY_TRACE(_T("i = %d, read = %d, level = %f\n"), i, read, sample.level);

		if (read < length)
			break;
	}

	media->close();
}

void MainWindow::fireReceive(const CachePtr& cache)
{
	MY_TRACE(_T("MainWindow::fireReceive()\n"));

	if (m_mutex && m_fileMapping)
	{
//		Synchronizer sync(*m_mutex);
		Bottle* shared = (Bottle*)m_fileMapping->getBuffer();

		shared->sampleCount = (int32_t)cache->samples.size();
		memcpy(shared->samples, cache->samples.data(), sizeof(Sample) * cache->samples.size());
	}
	else
	{
		return;
	}

	::SendMessage(g_parent, WM_AVIUTL_FILTER_RECEIVE, 0, 0);
}

//--------------------------------------------------------------------

inline float MainWindow::normalize(short pcm)
{
	return pcm / 32768.0f; // -1.0 ～ 1.0に正規化
}
#if 0
float MainWindow::calc(const std::vector<short>& samples)
{
	float level = 0.0f;

	for (short sample : samples)
	{
		float n = normalize(sample);

		level = std::max(level, fabsf(n));
	}

	return level;
}
#else
float MainWindow::calc(const std::vector<short>& samples)
{
	int c = (int)samples.size();

	if (!c)
		return 0.0f;

	float level = 0.0f;

	for (short sample : samples)
	{
		float n = normalize(sample);

		level += n * n;
	}

	return sqrtf(level / (float)c);
}
#endif
//--------------------------------------------------------------------

LRESULT MainWindow::onCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onCreate()\n"));

	m_hwnd = hwnd;

	{
		HWND hwnd = g_parent;

		m_mutex.reset(new Mutex(0, FALSE, FormatText(_T("ShowWaveform.Mutex.%08X"), hwnd)));
		m_fileMapping.reset(new SimpleFileMapping(sizeof(Bottle), FormatText(_T("ShowWaveform.FileMapping.%08X"), hwnd)));
	}

	TCHAR fileName[MAX_PATH] = {};
	::GetModuleFileName(0, fileName, MAX_PATH);
	::PathRemoveFileSpec(fileName);
	::PathAppend(fileName, _T("../lwinput.aui"));
	MY_TRACE_TSTR(fileName);
	
	plugin = std::make_shared<Input::Plugin>();
	plugin->load(fileName);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onDestroy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onDestroy()\n"));

	plugin->unload();
	plugin = 0;

	m_mutex.reset();
	m_fileMapping.reset();

	::PostQuitMessage(0);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onAviUtlFilterExit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterExit(0x%08X, 0x%08X)\n"), wParam, lParam);

	::PostQuitMessage(0);

	return 0;
}

LRESULT MainWindow::onAviUtlFilterSend(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterSend(0x%08X, 0x%08X)\n"), wParam, lParam);

	BottlePtr bottle = getBottle();
	if (bottle)
	{
		CachePtr cache = createCache(bottle);
		if (cache)
			fireReceive(cache);
	}

	return 0;
}

LRESULT MainWindow::onAviUtlFilterClear(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterClear(0x%08X, 0x%08X)\n"), wParam, lParam);

	cacheMap.clear();

	return 0;
}

LRESULT CALLBACK MainWindow::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE: return g_mainWindow.onCreate(hwnd, message, wParam, lParam);
	case WM_DESTROY: return g_mainWindow.onDestroy(hwnd, message, wParam, lParam);
	}

	if (message == WM_AVIUTL_FILTER_EXIT) return g_mainWindow.onAviUtlFilterExit(hwnd, message, wParam, lParam);
	else if (message == WM_AVIUTL_FILTER_SEND) return g_mainWindow.onAviUtlFilterSend(hwnd, message, wParam, lParam);
	else if (message == WM_AVIUTL_FILTER_CLEAR) return g_mainWindow.onAviUtlFilterClear(hwnd, message, wParam, lParam);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------
