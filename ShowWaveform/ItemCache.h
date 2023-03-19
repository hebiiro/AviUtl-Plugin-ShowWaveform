#pragma once

#include "ShowWaveform.h"

//---------------------------------------------------------------------

struct AudioParams {
	std::string fileName;
	int32_t playBegin = 0;
	double playSpeed = 1.0;
	int32_t frameBegin = 0;
	int32_t frameEnd = 0;
};

struct ItemCache {
	SampleArray samples;
	AudioParamsPtr params;
};

struct ItemCacheManager {
	ItemCacheMap cacheMap;
	ItemCachePtr getCache(ExEdit::Object* object);
	BOOL update(BOOL send);
	BOOL update(BOOL send, ExEdit::Object* object);
	AudioParamsPtr getAudioParams(ExEdit::Object* object);
	BOOL isChanged(const ItemCachePtr& cache, ExEdit::Object* object);
};

//---------------------------------------------------------------------
