#pragma once

#include "ShowWaveform.h"

//---------------------------------------------------------------------

struct ItemCache {
	VolumeArray volumes;
	AudioParamsPtr params;
};

struct ItemCacheManager {
	ItemCacheMap cacheMap;
	ItemCachePtr getCache(int32_t object_index);
	BOOL update(BOOL send);
	ItemCachePtr update(BOOL send, int32_t object_index, ExEdit::Object* object);
	AudioParamsPtr getAudioParams(ExEdit::Object* object);
	BOOL isChanged(const ItemCachePtr& cache, ExEdit::Object* object);
	static void copyFileName(LPSTR dst, size_t size, LPCSTR src);

	//
	// 音声ファイルアイテムと連携している動画ファイルアイテムを返します。
	//
	static ExEdit::Object* get_movie_file_item(ExEdit::Object* object);
};

//---------------------------------------------------------------------
