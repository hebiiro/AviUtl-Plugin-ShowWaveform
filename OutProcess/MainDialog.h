#pragma once
#include "Resource.h"

struct MainDialog : public Tools::Dialog
{
	BOOL m_ignoreNotification = FALSE;

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

	BOOL create(HINSTANCE instance, HWND parent);
	void ignoreNotification(BOOL ignoreNotification);
	virtual INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterResize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};
