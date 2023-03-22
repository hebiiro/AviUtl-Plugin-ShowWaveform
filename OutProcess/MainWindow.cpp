#include "pch.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

MainWindow g_mainWindow;

//--------------------------------------------------------------------

BottlePtr MainWindow::getBottle()
{
	BottlePtr bottle = std::make_shared<Bottle>();

	{
//		Synchronizer sync(m_mutex);
		Bottle* shared = (Bottle*)m_fileMapping.getBuffer();

		if (!shared)
			return 0;

		memcpy(bottle.get(), shared, sizeof(*shared));
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

	MY_TRACE_HEX(ip);
	MY_TRACE_HEX(handle);

	int32_t start = 0;
	int32_t length = mi->audio_format.nSamplesPerSec / SAMPLE_FPS;
	int32_t bufferSize = mi->audio_format.nBlockAlign * length;
	std::vector<BYTE> buffer(bufferSize);

	MY_TRACE_INT(mi->audio_format.wFormatTag);
	MY_TRACE_INT(mi->audio_format.nChannels);
	MY_TRACE_INT(mi->audio_format.nSamplesPerSec);
	MY_TRACE_INT(mi->audio_format.nAvgBytesPerSec);
	MY_TRACE_INT(mi->audio_format.nBlockAlign);
	MY_TRACE_INT(mi->audio_format.wBitsPerSample);
	MY_TRACE_INT(length);
	MY_TRACE_INT(bufferSize);

	for (int i = 0; ; i++)
	{
		int32_t read = ip->func_read_audio(handle, start, length, buffer.data());
		start += read;

		Sample sample;
		switch (mi->audio_format.wBitsPerSample)
		{
		case 8: sample.level = calc((const char*)buffer.data(), buffer.size() / sizeof(char)); break;
		case 16: sample.level = calc((const short*)buffer.data(), buffer.size() / sizeof(short)); break;
		case 32: sample.level = calc((const long*)buffer.data(), buffer.size() / sizeof(long)); break;
		}
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

	{
//		Synchronizer sync(m_mutex);
		Bottle* shared = (Bottle*)m_fileMapping.getBuffer();

		if (!shared)
			return;

		shared->sampleCount = (int32_t)cache->samples.size();
		memcpy(shared->samples, cache->samples.data(), sizeof(Sample) * cache->samples.size());
	}

	::SendMessage(g_parent, WM_AVIUTL_FILTER_RECEIVE, 0, 0);
}

//--------------------------------------------------------------------

inline float MainWindow::normalize(char pcm)
{
	return pcm / 128.0f; // 8bit を -1.0 ～ 1.0 に正規化
}

inline float MainWindow::normalize(short pcm)
{
	return pcm / 32768.0f; // 16bit を -1.0 ～ 1.0 に正規化
}

inline float MainWindow::normalize(long pcm)
{
//	return pcm / 8388608.0f; // 24bit を -1.0 ～ 1.0 に正規化
	return pcm / 2147483648.0f; // 32bit を -1.0 ～ 1.0 に正規化
}

inline float MainWindow::normalize(float pcm)
{
	return pcm;
}
#if 0
template<typename T>
float MainWindow::calc(const T* samples, int count)
{
	float level = 0.0f;
	for (int i = 0; i < count; i++)
	{
		T sample = samples[i];
		float n = normalize(sample);
		level = std::max(level, fabsf(n));
	}
	return level;
}
#else
template<typename T>
float MainWindow::calc(const T* samples, int count)
{
	if (!count) return 0.0f;

	float level = 0.0f;
	for (int i = 0; i < count; i++)
	{
		T sample = samples[i];
		float n = normalize(sample);
		level += n * n;
	}
	return sqrtf(level / (float)count);
}
#endif
//--------------------------------------------------------------------

LRESULT MainWindow::onCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onCreate()\n"));

	m_hwnd = hwnd;
	m_mutex.init(0, FALSE, FormatText(_T("ShowWaveform.Mutex.%08X"), g_parent));
	m_fileMapping.init(sizeof(Bottle), FormatText(_T("ShowWaveform.FileMapping.%08X"), g_parent));

	TCHAR fileName[MAX_PATH] = {};
	::GetModuleFileName(0, fileName, MAX_PATH);
	::PathRemoveFileSpec(fileName);
	::PathAppend(fileName, _T("..\\lwinput.aui"));
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
