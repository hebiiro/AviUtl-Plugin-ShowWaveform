#pragma once

//---------------------------------------------------------------------

// OutProcess => InProcess
static const UINT WM_AVIUTL_FILTER_INIT = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_INIT"));
static const UINT WM_AVIUTL_FILTER_RECEIVE = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_RECEIVE"));

// InProcess => OutProcess
static const UINT WM_AVIUTL_FILTER_EXIT = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_EXIT"));
static const UINT WM_AVIUTL_FILTER_SEND = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_SEND"));
static const UINT WM_AVIUTL_FILTER_CLEAR = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_CLEAR"));

//---------------------------------------------------------------------

static const int32_t SAMPLE_FPS = 30;
static const int32_t MAX_SAMPLE_COUNT = SAMPLE_FPS * 60 * 60 * 10;

struct Sample {
	float level;
};

struct Bottle {
	char fileName[MAX_PATH];
	int32_t sampleCount;
	Sample samples[MAX_SAMPLE_COUNT];
};

//---------------------------------------------------------------------
