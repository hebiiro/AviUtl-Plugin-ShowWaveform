#include "pch.h"
#include "CacheManager.h"
#include "App.h"

//--------------------------------------------------------------------

CachePtr CacheManager::getCache(LPCSTR fileName)
{
	MY_TRACE(_T("CacheManager::getCache(%hs)\n"), fileName);

	auto it = cacheMap.find(fileName);
	if (it == cacheMap.end()) return 0;
	return it->second;
}

// リーダーがボトルに詰め込んだ音量からキャッシュを作成する。
CachePtr CacheManager::createCache(const ReaderPtr& reader)
{
	MY_TRACE(_T("CacheManager::createCache()\n"));

	ReaderBottle* shared = reader->getBottle();
	if (!shared) return 0;

	MY_TRACE_STR(shared->fileName);

	CachePtr cache = std::make_shared<Cache>();
	cache->fileName = shared->fileName;
	cache->volumes.insert(cache->volumes.end(),
		shared->volumes, shared->volumes + shared->volumeCount);
	cacheMap[cache->fileName] = cache;
	return cache;
}

void CacheManager::clear()
{
	cacheMap.clear();
}

//--------------------------------------------------------------------
