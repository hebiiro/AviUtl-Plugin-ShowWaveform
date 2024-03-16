#pragma once

#include "OutProcess.h"
#include "ReaderManager.h"

//--------------------------------------------------------------------

struct Cache {
	std::string fileName;
	std::vector<Volume> volumes;
};

typedef std::shared_ptr<Cache> CachePtr;
typedef std::map<std::string, CachePtr> CacheMap;

//--------------------------------------------------------------------

struct CacheManager {
	CacheMap cacheMap;

	CachePtr getCache(LPCSTR fileName);
	CachePtr createCache(const ReaderPtr& reader);
	void clear();
};

//--------------------------------------------------------------------
