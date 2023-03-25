#pragma once

//---------------------------------------------------------------------

// OutProcess => InProcess
static const UINT WM_AVIUTL_FILTER_INIT = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_INIT"));
static const UINT WM_AVIUTL_FILTER_RECEIVE = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_RECEIVE"));
static const UINT WM_AVIUTL_FILTER_CHANGE_FRAME = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_CHANGE_FRAME"));

// InProcess => OutProcess
static const UINT WM_AVIUTL_FILTER_EXIT = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_EXIT"));
static const UINT WM_AVIUTL_FILTER_RESIZE = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_RESIZE"));
static const UINT WM_AVIUTL_FILTER_SEND = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_SEND"));
static const UINT WM_AVIUTL_FILTER_CLEAR = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_CLEAR"));
static const UINT WM_AVIUTL_FILTER_REDRAW = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_REDRAW"));

struct SendID {
	static const int requestCache = 1;
	static const int notifyProjectChanged = 2;
	static const int notifyItemChanged = 3;
};

//---------------------------------------------------------------------

static const int32_t SAMPLE_FPS = 30;
static const int32_t MAX_SAMPLE_COUNT = SAMPLE_FPS * 60 * 60 * 4; // 最大 4 時間

struct Sample {
	float level;
};

struct Bottle {
	char fileName[MAX_PATH];
	int32_t sampleCount;
	Sample samples[MAX_SAMPLE_COUNT];
};

struct ProjectParams {
	int32_t video_scale;
	int32_t video_rate;
	int32_t frameNumber;
	int32_t sceneIndex;
	int32_t currentFrame;
};

struct AudioParams {
	uint32_t id = 0;
	uint32_t flag = 0;
	int32_t frameBegin = 0;
	int32_t frameEnd = 0;
	int32_t sceneSet = 0;
	char fileName[MAX_PATH] = {};
	float volume = 1.0;
	int32_t playBegin = 0;
	float playSpeed = 1.0;
};

//---------------------------------------------------------------------
