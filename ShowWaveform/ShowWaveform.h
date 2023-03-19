#pragma once

#include "Share.h"

//---------------------------------------------------------------------

struct FileCache;
struct FileCacheManager;
struct ItemCache;
struct ItemCacheManager;
struct AudioParams;

typedef std::vector<Sample> SampleArray;

typedef std::shared_ptr<FileCache> FileCachePtr;
typedef std::map<std::string, FileCachePtr> FileCacheMap;

typedef std::shared_ptr<ItemCache> ItemCachePtr;
typedef std::map<ExEdit::Object*, ItemCachePtr> ItemCacheMap;

typedef std::shared_ptr<AudioParams> AudioParamsPtr;

//---------------------------------------------------------------------

extern int TRACK_SCALE;
extern int TRACK_SHOW_TYPE;
extern int TRACK_UPDATE_MODE;

extern int CHECK_CLEAR;
extern int CHECK_SHOW_WAVEFORM;
extern int CHECK_SHOW_TEXT;
extern int CHECK_NO_SCROLL_TEXT;

//---------------------------------------------------------------------
