#pragma once

#include "ShowWaveform.h"

//---------------------------------------------------------------------

struct ItemCache {
	SampleArray samples;
	AudioParamsPtr params;
};

struct ItemCacheManager {
	ItemCacheMap cacheMap;
	ItemCachePtr getCache(ExEdit::Object* object);
	BOOL update(BOOL send);
	ItemCachePtr update(BOOL send, ExEdit::Object* object);
	AudioParamsPtr getAudioParams(ExEdit::Object* object);
	BOOL isChanged(const ItemCachePtr& cache, ExEdit::Object* object);
	static void copyFileName(LPSTR dst, size_t size, LPCSTR src);
};

//---------------------------------------------------------------------
