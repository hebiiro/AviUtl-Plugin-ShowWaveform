#include "pch.h"
#include "FileCache.h"
#include "App.h"

//---------------------------------------------------------------------

FileCachePtr FileCacheManager::getCache(LPCSTR fileName, BOOL create)
{
	if (strlen(fileName) == 0)
		return 0;

	auto it = cacheMap.find(fileName);
	if (it != cacheMap.end())
	{
		if (it->second->video_scale == theApp.m_fi.video_scale &&
			it->second->video_rate == theApp.m_fi.video_rate)
		{
			return it->second; // キャッシュが見つかったのでそれを返す。
		}
	}

	if (create)
	{
		// キャッシュが存在しないので作成する。
		createCache(fileName);
	}

	return 0;
}

BOOL FileCacheManager::createCache(LPCSTR fileName)
{
	MY_TRACE(_T("FileCacheManager::createCache(%hs)\n"), fileName);

	// サブスレッドマネージャにキャッシュの作成を要請する。
	return theApp.m_subThreadManager.requestCache(fileName);
}

BOOL FileCacheManager::receiveCache()
{
	MY_TRACE(_T("FileCacheManager::receiveCache()\n"));

	// 受け取り用のボトルを取得する。
	ReceiverBottle* shared = ::shared.receiverBottle.getBuffer();
	if (!shared) return FALSE;

	MY_TRACE_STR(shared->fileName);
	MY_TRACE_INT(shared->sampleCount);

	// キャッシュを更新する。
	FileCachePtr cache = std::make_shared<FileCache>();
	cache->video_scale = theApp.m_fi.video_scale;
	cache->video_rate = theApp.m_fi.video_rate;
	cache->samples.insert(cache->samples.end(),
		shared->samples, shared->samples + shared->sampleCount);
	cacheMap[shared->fileName] = cache;

	return TRUE;
}

//---------------------------------------------------------------------
