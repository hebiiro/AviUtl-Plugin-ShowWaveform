#pragma once

#include "ShowWaveform.h"

//---------------------------------------------------------------------

struct FileCache {
	int32_t video_scale;
	int32_t video_rate;
	SampleArray samples;
};

struct FileCacheManager {
	FileCacheMap cacheMap;
	FileCachePtr getCache(LPCSTR fileName, BOOL create);
	void createCache(LPCSTR fileName);
	void receiveCache();
};

//---------------------------------------------------------------------
