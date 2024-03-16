#pragma once

#include "CacheManager.h"
#include "ReaderManager.h"
#include "WaiterManager.h"
#include "MainWindow.h"
#include "MainDialog.h"

//--------------------------------------------------------------------

typedef std::map<uint32_t, AudioParamsPtr> AudioParamsMap;

struct Total {
	float level;
	float rms;
};

typedef std::vector<Total> Totals;

//--------------------------------------------------------------------

extern Shared shared;

//--------------------------------------------------------------------

struct App {
	HINSTANCE instance = 0;
	HWND windowContainer = 0;
	HWND dialogContainer = 0;
	CacheManager cacheManager;
	ReaderManager readerManager;
	WaiterManager waiterManager;
	MainWindow mainWindow;
	MainDialog mainDialog;

	BOOL getShowBPM();
	BOOL setShowBPM(BOOL newShowBPM);
	int getOrig();
	BOOL setOrig(int newOrig);
	int getBPM();
	BOOL setBPM(int newBPM);
	int getAbove();
	BOOL setAbove(int newAbove);
	int getBelow();
	BOOL setBelow(int newBelow);

	int getLimitVolume();
	BOOL setLimitVolume(int newLimitVolume);
	int getBaseVolume();
	BOOL setBaseVolume(int newBaseVolume);
	int getMinRMS();
	BOOL setMinRMS(int newMinRMS);
	int getMaxRMS();
	BOOL setMaxRMS(int newMaxRMS);
	int getBaseRMS();
	BOOL setBaseRMS(int newBaseRMS);
	int getZoom();
	BOOL setZoom(int newZoom);

	static void CALLBACK timerProc(HWND hwnd, UINT message, UINT_PTR timerId, DWORD time);
	int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow);
	LRESULT onAviUtlFilterSend(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterReceive(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterClear(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterRedraw(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	// App_Waveform

	Totals totals;
	ProjectParamsPtr projectParams;
	AudioParamsMap audioParamsMap;

	BOOL sendCache(const CachePtr& cache);
	BOOL recalcTotals();
	BOOL setProjectParams(const ProjectParamsPtr& params);
	BOOL setAudioParams(const AudioParamsPtr& params);
	BOOL setTotalsParams(const TotalsParamsPtr& params);
};

extern App theApp;

//--------------------------------------------------------------------
