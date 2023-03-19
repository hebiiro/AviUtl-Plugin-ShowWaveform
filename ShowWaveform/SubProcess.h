#pragma once

//--------------------------------------------------------------------

struct SubProcess
{
	PROCESS_INFORMATION m_pi = {};
	HWND m_mainWindow = 0;

	BOOL init(AviUtl::FilterPlugin* fp);
	BOOL exit(AviUtl::FilterPlugin* fp);
};

//--------------------------------------------------------------------
