#pragma once

#include "FileCache.h"
#include "ItemCache.h"
#include "SubProcess.h"
#include "SubThread.h"

//---------------------------------------------------------------------
// Api Hook

DECLARE_HOOK_PROC(void, CDECL, DrawObject, (HDC dc, int objectIndex));

BOOL WINAPI drawObjectText(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx);

//---------------------------------------------------------------------

struct App
{
	AviUtlInternal m_auin;

	HINSTANCE m_instance = 0;
	AviUtl::FilterPlugin* m_fp = 0;
	AviUtl::FileInfo m_fi = {};
	ExEdit::Object* m_currentDrawObject = 0;
	FileCacheManager m_fileCacheManager;
	ItemCacheManager m_itemCacheManager;
	SubProcess m_subProcess;
	SubThreadManager m_subThreadManager;

	COLORREF m_penColor = RGB(0x00, 0xff, 0xff);
	COLORREF m_brushColor = RGB(0x00, 0xff, 0xff);

	int (*CallShowColorDialog)(DWORD u1, COLORREF* color, DWORD u3) = 0;

	App();
	~App();

	BOOL DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved);
	void load(int* track_def, int* check_def);
	void save(int* track_def, int* check_def);
	BOOL func_init(AviUtl::FilterPlugin* fp);
	BOOL func_exit(AviUtl::FilterPlugin* fp);
	BOOL func_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip);
	BOOL func_update(AviUtl::FilterPlugin* fp, AviUtl::FilterPlugin::UpdateStatus status);
	BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp);

	void drawWaveform(HDC dc, LPCRECT rcClip, LPCRECT rcItem);
};

extern App theApp;

//---------------------------------------------------------------------
