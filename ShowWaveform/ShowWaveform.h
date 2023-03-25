#pragma once

#include "Share.h"

//---------------------------------------------------------------------

struct FileCache;
struct FileCacheManager;
struct ItemCache;
struct ItemCacheManager;

typedef std::vector<Sample> SampleArray;

typedef std::shared_ptr<FileCache> FileCachePtr;
typedef std::map<std::string, FileCachePtr> FileCacheMap;

typedef std::shared_ptr<ItemCache> ItemCachePtr;
typedef std::map<ExEdit::Object*, ItemCachePtr> ItemCacheMap;

typedef std::shared_ptr<Bottle> BottlePtr;
typedef std::shared_ptr<ProjectParams> ProjectParamsPtr;
typedef std::shared_ptr<AudioParams> AudioParamsPtr;

//---------------------------------------------------------------------
