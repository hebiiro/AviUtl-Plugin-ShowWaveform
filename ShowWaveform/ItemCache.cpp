#include "pch.h"
#include "ItemCache.h"
#include "App.h"

//---------------------------------------------------------------------

ItemCachePtr ItemCacheManager::getCache(int32_t object_index)
{
	auto it = cacheMap.find(object_index);
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
		auto object = theApp.m_auin.GetObject(it->first);

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

		// オブジェクトのインデックスを取得します。
		auto object_index = theApp.m_auin.get_object_index(object);
		if (object_index < 0) continue;

		// すでにキャッシュが作成済みなら
		auto it = cacheMap.find(object_index);
		if (it != cacheMap.end())
		{
			ItemCachePtr cache = it->second;

			// オブジェクトの状態を調べる。
			if (!isChanged(cache, object))
				continue; // オブジェクトの状態に変化がないときは何もしない。
		}

		// キャッシュを更新する。
		ItemCachePtr cache = update(send, object_index, object);
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

	// 更新が必要なアイテムが存在しない場合はここで処理終了。
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

ItemCachePtr ItemCacheManager::update(BOOL send, int32_t object_index, ExEdit::Object* object)
{
	MY_TRACE(_T("ItemCacheManager::update(%d, %d, 0x%08X)\n"), send, object_index, object);

	// 音声アイテムのパラメータを取得する。
	AudioParamsPtr params = getAudioParams(object);
	if (!params) return nullptr;

	// このオブジェクトのキャッシュが要求されているかチェックする。
	if (!theApp.isCacheRequired(object, params)) return nullptr;

	// ファイルキャシュを取得する。
	FileCachePtr fileCache = theApp.m_fileCacheManager.getCache(params->fileName, send);
	if (!fileCache) return nullptr; // ファイルキャッシュが作成されるまでは何もできない。

	// アイテムキャッシュを作成する。
	ItemCachePtr itemCache = std::make_shared<ItemCache>();
	cacheMap[object_index] = itemCache;

	// オブジェクトの状態を保存しておく。
	itemCache->params = params;

	// 現在のフレームレートの1フレーム毎の音量に変換する。

	double scale = (double)Volume::Resolution * theApp.m_fi.video_scale / theApp.m_fi.video_rate;
	int32_t range = object->frame_end - object->frame_begin;
	for (int32_t i = 0; i <= range; i++)
	{
		double temp1 = scale * params->playSpeed * i;
		double temp2 = scale * params->playBegin;
		int32_t src = (int32_t)(temp1 + temp2);

		if (src >= (int32_t)fileCache->volumes.size())
			break;

		Volume volume = fileCache->volumes[src];
		volume.level *= params->volume;
		itemCache->volumes.emplace_back(volume);
	}

	return itemCache;
}

AudioParamsPtr ItemCacheManager::getAudioParams(ExEdit::Object* object)
{
	int audioFileIndex = theApp.m_auin.GetFilterIndex(object, 2); // 音声ファイル
	if (audioFileIndex == -1) return 0;

	int standardPlayIndex = theApp.m_auin.GetFilterIndex(object, 12); // 標準再生
	if (standardPlayIndex == -1) return 0;

	int volumeIndex = object->filter_param[standardPlayIndex].track_begin + 0;
	int panIndex = object->filter_param[standardPlayIndex].track_begin + 1;
	int playBeginIndex = object->filter_param[audioFileIndex].track_begin + 0;
	int playSpeedIndex = object->filter_param[audioFileIndex].track_begin + 1;

	AudioParamsPtr params = std::make_shared<AudioParams>();
	params->id = (uint32_t)object;
	params->flag = (uint32_t)object->flag;
	params->frameBegin = object->frame_begin;
	params->frameEnd = object->frame_end;
	params->sceneSet = object->scene_set;
	params->volume = object->track_value_left[volumeIndex] / 1000.0f;

	ExEdit::LayerSetting* layer = theApp.m_auin.GetLayerSetting(object->layer_set);
	params->layerFlag = (uint32_t)layer->flag;

	// 動画ファイルと連携している場合は動画ファイルから再生位置を取得します。
	if (object->check_value[1])
	{
		if (auto object2 = get_movie_file_item(object))
		{
			auto playBeginIndex = object2->filter_param[0].track_begin + 0;
			auto playSpeedIndex = object2->filter_param[0].track_begin + 1;

			// 拡張データを取得する。
			auto exdata = (ExEdit::Exdata::efMovieFile*)theApp.m_auin.GetExdata(object2, 0);

			copyFileName(params->fileName, sizeof(params->fileName), exdata->file);
			params->playBegin = object2->track_value_left[playBeginIndex];
			params->playSpeed = object2->track_value_left[playSpeedIndex] / 1000.0f;
		}
	}
	// それ以外の場合は音声ファイルから再生位置を取得します。
	else
	{
		// 拡張データを取得する。
		auto exdata = (ExEdit::Exdata::efAudioFile*)theApp.m_auin.GetExdata(object, 0);

		copyFileName(params->fileName, sizeof(params->fileName), exdata->file);
		params->playBegin = object->track_value_left[playBeginIndex] * theApp.m_fi.video_rate / theApp.m_fi.video_scale / 100;
		params->playSpeed = object->track_value_left[playSpeedIndex] / 1000.0f;
	}

	return params;
}

BOOL ItemCacheManager::isChanged(const ItemCachePtr& cache, ExEdit::Object* object)
{
	AudioParamsPtr params = getAudioParams(object);
	if (!params) return FALSE;

	if (strcmp(cache->params->fileName, params->fileName) != 0) return TRUE;
	if (cache->params->volume != params->volume) return TRUE;
	if (cache->params->playBegin != params->playBegin) return TRUE;
	if (cache->params->playSpeed != params->playSpeed) return TRUE;
	if (cache->params->frameEnd != params->frameEnd) return TRUE;
	if (cache->params->layerFlag != params->layerFlag) return TRUE;

	return FALSE;
}

void ItemCacheManager::copyFileName(LPSTR dst, size_t size, LPCSTR src)
{
	// フルパスを取得する。
	::GetFullPathNameA(src, size / sizeof(*dst), dst, 0);

	// 小文字に変換する。
	size_t length = strlen(dst);
	for (size_t i = 0; i < length; i++)
	{
		if (::IsDBCSLeadByte(dst[i]))
			i++;
		else
			dst[i] = tolower(dst[i]);
	}
}

//---------------------------------------------------------------------
