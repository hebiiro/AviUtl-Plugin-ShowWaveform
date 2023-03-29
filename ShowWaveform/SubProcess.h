#pragma once

//--------------------------------------------------------------------

struct SubProcess
{
	PROCESS_INFORMATION m_pi = {};
	HWND m_fullSamplesWindow = 0;

	BOOL init(AviUtl::FilterPlugin* fp);
	BOOL exit(AviUtl::FilterPlugin* fp);

	struct Window {
		static const int TimerID = 1000;
		HWND m_hwnd = 0;
		BOOL init(AviUtl::FilterPlugin* fp);
		BOOL exit(AviUtl::FilterPlugin* fp);
		void delayedUpdate();
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	} m_fullSamplesContainer;
};

//--------------------------------------------------------------------
