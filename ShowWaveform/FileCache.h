#pragma once

#include "ShowWaveform.h"

//---------------------------------------------------------------------

struct FileCache {
	int32_t video_scale;
	int32_t video_rate;
	VolumeArray volumes;
};

struct FileCacheManager {
	FileCacheMap cacheMap;
	FileCachePtr getCache(LPCSTR fileName, BOOL create);
	BOOL createCache(LPCSTR fileName);
	BOOL receiveCache();
};

//---------------------------------------------------------------------
