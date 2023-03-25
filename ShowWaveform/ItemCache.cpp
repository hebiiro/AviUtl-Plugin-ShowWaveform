﻿#include "pch.h"
#include "ItemCache.h"
#include "App.h"

//---------------------------------------------------------------------

ItemCachePtr ItemCacheManager::getCache(ExEdit::Object* object)
{
	auto it = cacheMap.find(object);
	if (it == cacheMap.end()) return 0;
	return it->second;
}

BOOL ItemCacheManager::update(BOOL send)
{
	MY_TRACE(_T("ItemCacheManager::update(%d)\n"), send);

	std::vector<AudioParamsPtr> updateItems;

	// 無効なキャッシュを削除する。
	for (auto it = cacheMap.begin(); it != cacheMap.end();)
	{
		ExEdit::Object* object = it->first;

		if (!(object->flag & ExEdit::Object::Flag::Exist) ||
			!(object->flag & ExEdit::Object::Flag::Sound))
		{
			it->second->params->flag = (uint32_t)object->flag;
			updateItems.emplace_back(it->second->params);
			cacheMap.erase(it++);
		}
		else
		{
			it++;
		}
	}

	BOOL isCacheCompleted = TRUE;

	// 現在のシーン内のアイテムを列挙する。
	int c = theApp.m_auin.GetCurrentSceneObjectCount();
	for (int i = 0; i < c; i++)
	{
		// オブジェクトを取得する。
		ExEdit::Object* object = theApp.m_auin.GetSortedObject(i);

		// オブジェクトのフラグを調べる。
		if (!(object->flag & ExEdit::Object::Flag::Sound))
			continue; // 音声波形を取得できるのはサウンドタイプのアイテムのみ。

		// すでにキャッシュが作成済みなら
		auto it = cacheMap.find(object);
		if (it != cacheMap.end())
		{
			ItemCachePtr cache = it->second;

			// オブジェクトの状態を調べる。
			if (!isChanged(cache, object))
				continue; // オブジェクトの状態に変化がないときは何もしない。
		}

		// キャッシュを更新する。
		ItemCachePtr cache = update(send, object);
		if (cache)
		{
			updateItems.emplace_back(cache->params);
			theApp.m_auin.RedrawLayer(object->layer_set);
		}
		else
		{
			// キャッシュをコンプリートできなかった。
			isCacheCompleted = FALSE;
		}
	}

	if (updateItems.empty())
		return FALSE;

	// 更新が必要なアイテムを通知する。
	for (auto& item : updateItems)
		theApp.m_subThreadManager.notifyItemChanged(item);

	// キャッシュをコンプリートできている場合のみ再描画をリクエストする。
	if (isCacheCompleted)
		theApp.m_subThreadManager.requestRedraw();

	return TRUE;
}

ItemCachePtr ItemCacheManager::update(BOOL send, ExEdit::Object* object)
{
	MY_TRACE(_T("ItemCacheManager::update(%d, 0x%08X)\n"), send, object);

	// 音声アイテムのパラメータを取得する。
	AudioParamsPtr params = getAudioParams(object);

	// ファイルキャシュを取得する。
	FileCachePtr fileCache = theApp.m_fileCacheManager.getCache(params->fileName, send);
	if (!fileCache) return 0; // ファイルキャッシュが作成されるまでは何もできない。

	// アイテムキャッシュを作成する。
	ItemCachePtr itemCache = std::make_shared<ItemCache>();
	cacheMap[object] = itemCache;

	// オブジェクトの状態を保存しておく。
	itemCache->params = params;

	// 現在のフレームレートの1フレーム毎のサンプルに変換する。

	double scale = (double)SAMPLE_FPS * theApp.m_fi.video_scale / theApp.m_fi.video_rate;
	int32_t range = object->frame_end - object->frame_begin;
	for (int32_t i = 0; i <= range; i++)
	{
		double temp1 = scale * params->playSpeed * i;
		double temp2 = scale * params->playBegin;
		int32_t src = (int32_t)(temp1 + temp2);

		if (src >= (int32_t)fileCache->samples.size())
			break;

		itemCache->samples.emplace_back(fileCache->samples[src]);
	}

	return itemCache;
}

AudioParamsPtr ItemCacheManager::getAudioParams(ExEdit::Object* object)
{
	AudioParamsPtr params = std::make_shared<AudioParams>();
	params->id = (uint32_t)object;
	params->flag = (uint32_t)object->flag;
	params->frameBegin = object->frame_begin;
	params->frameEnd = object->frame_end;
	params->sceneSet = object->scene_set;
	params->volume = object->track_value_left[2] / 1000.0f;

	if (object->check_value[1])
	{
		// 同じグループの動画アイテムを探す。
		int c = theApp.m_auin.GetObjectCount();
		for (int i = 0; i < c; i++)
		{
			ExEdit::Object* object2 = theApp.m_auin.GetObjectA(i);
			if (!object2) continue;
			if (object2->group_belong != object->group_belong) continue;
			if (object2->scene_set != object->scene_set) continue;
			if (object2->exdata_size != 288) continue;

			// 拡張データを取得する。
			BYTE* exdata = theApp.m_auin.GetExdata(object2, 0);

			::StringCbCopyA(params->fileName, sizeof(params->fileName), (LPCSTR)exdata);
			params->playBegin = object2->track_value_left[0];
			params->playSpeed = object2->track_value_left[1] / 1000.0f;

			break;
		}
	}
	else
	{
		// 拡張データを取得する。
		BYTE* exdata = theApp.m_auin.GetExdata(object, 0);

		::StringCbCopyA(params->fileName, sizeof(params->fileName), (LPCSTR)exdata);
		params->playBegin = object->track_value_left[0] * theApp.m_fi.video_rate / theApp.m_fi.video_scale / 100;
		params->playSpeed = object->track_value_left[1] / 1000.0f;
	}

	return params;
}

BOOL ItemCacheManager::isChanged(const ItemCachePtr& cache, ExEdit::Object* object)
{
	AudioParamsPtr params = getAudioParams(object);

	if (strcmp(cache->params->fileName, params->fileName) != 0) return TRUE;
	if (cache->params->volume != params->volume) return TRUE;
	if (cache->params->playBegin != params->playBegin) return TRUE;
	if (cache->params->playSpeed != params->playSpeed) return TRUE;
	if (cache->params->frameEnd != params->frameEnd) return TRUE;

	return FALSE;
}

//---------------------------------------------------------------------
