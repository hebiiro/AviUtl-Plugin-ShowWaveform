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

struct XORMode {
	static const int None = 0;
	static const int XOR = 1;
	static const int NotXOR = 2;
	static const int Not = 3;
};

struct Track {
	static const int Scale = 0;
	static const int ShowType = 1;
	static const int UpdateMode = 2;
	static const int XORMode = 3;
};

struct Check {
	static const int PenColor = 0;
	static const int BrushColor = 1;
	static const int Clear = 2;
	static const int ShowWaveform = 3;
	static const int ShowText = 4;
	static const int NoScrollText = 5;
};

//---------------------------------------------------------------------
