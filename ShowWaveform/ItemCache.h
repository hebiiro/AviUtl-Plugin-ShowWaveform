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
	inline static ExEdit::Object* get_movie_file_item(ExEdit::Object* object)
	{
		auto result = (ExEdit::Object*)nullptr;
		auto distance = 100;

		// 同じ開始位置で最も近いレイヤーにある動画アイテムを探します。
		auto c = theApp.m_auin.GetCurrentSceneObjectCount();
		for (decltype(c) i = 0; i < c; i++)
		{
			auto object2 = theApp.m_auin.GetSortedObject(i);

			// シーンが異なるアイテムは無視します。
			if (object2->scene_set != object->scene_set) continue;

			// 開始位置が異なるアイテムは無視します。
			if (object2->frame_begin != object->frame_begin) continue;

			// レイヤー位置が下のアイテムは無視します。
			if (object2->layer_set >= object->layer_set) continue;

			// レイヤー位置が遠い場合は無視します。
			auto distance2 = object->layer_set - object2->layer_set;
			if (distance2 > distance) continue;
#if 0
			// 音声ファイルアイテムの場合は
			if (object2->filter_param[0].id == c_audio_file_filter_id)
			{
				// この音声ファイルアイテムより下にある動画ファイルアイテムのみを連携対象とみなします。
				result = nullptr;
				distance = distance2;
			}
			else
#endif
			{
				// 動画ファイル(id=0)以外のアイテムは無視します。
				if (object2->filter_param[0].id != 0) continue;

				// この動画ファイルアイテムを連携対象とします。
				result = object2;
				distance = distance2;
			}
		}

		return result;
	}
};

//---------------------------------------------------------------------
