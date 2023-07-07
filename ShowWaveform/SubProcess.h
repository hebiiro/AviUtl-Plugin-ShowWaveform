#pragma once

#include "ShowWaveform.h"

//--------------------------------------------------------------------

struct SubProcess
{
	PROCESS_INFORMATION m_pi = {};

	BOOL init(AviUtl::FilterPlugin* fp);
	BOOL exit(AviUtl::FilterPlugin* fp);

	struct Container {
		HWND m_hwnd = 0;
		HWND m_inner = 0;
		BOOL init(AviUtl::FilterPlugin* fp, HWND owner, LPCTSTR windowText);
		BOOL exit(AviUtl::FilterPlugin* fp);
		virtual LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LPCTSTR getPropName(){ return _T("SubProcess.Container"); }
	};

	struct WindowContainer : Container {
		struct TimerID {
			static const int Update = 1000;
			static const int SendFullSamplesParams = 1001;
		};
		FullSamplesParams m_delayedFullSampleParams;
		void delayedUpdate();
		void delayedSendFullSamplesParams(const FullSamplesParams* params);
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	} m_windowContainer;

	struct DialogContainer : Container {
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	} m_dialogContainer;
};

//--------------------------------------------------------------------
