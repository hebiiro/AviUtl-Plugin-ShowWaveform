#pragma once

#include "Share.h"

//---------------------------------------------------------------------

struct FileCache;
struct FileCacheManager;
struct ItemCache;
struct ItemCacheManager;

typedef std::vector<Volume> VolumeArray;

typedef std::shared_ptr<FileCache> FileCachePtr;
typedef std::map<std::string, FileCachePtr> FileCacheMap;

typedef std::shared_ptr<ItemCache> ItemCachePtr;
typedef std::map<int32_t, ItemCachePtr> ItemCacheMap;

typedef std::shared_ptr<SenderBottle> SenderBottlePtr;
typedef std::shared_ptr<ReceiverBottle> ReceiverBottlePtr;
typedef std::shared_ptr<ProjectParams> ProjectParamsPtr;
typedef std::shared_ptr<AudioParams> AudioParamsPtr;
typedef std::shared_ptr<TotalsParams> TotalsParamsPtr;

//---------------------------------------------------------------------
