#pragma once

#include "FileCache.h"
#include "ItemCache.h"
#include "SubProcess.h"
#include "SubThread.h"
#include "MainDialog.h"

//--------------------------------------------------------------------
// Api Hook

DECLARE_HOOK_PROC(void, CDECL, DrawObject, (HDC dc, int objectIndex));
DECLARE_HOOK_PROC(BOOL, WINAPI, DrawObjectText, (HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx));

//--------------------------------------------------------------------

struct App
{
	struct ShowType {
		static const int Both = 0;
		static const int Min = 1;
		static const int Max = 2;
	};

	struct UpdateMode {
		static const int Off = 0;
		static const int On = 1;
		static const int OnWithoutPlaying = 2;
	};

	struct XORMode {
		static const int None = 0;
		static const int XOR = 1;
		static const int NotXOR = 2;
		static const int Not = 3;
	};

	AviUtlInternal m_auin;
	HINSTANCE m_instance = 0;
	AviUtl::FilterPlugin* m_fp = 0;
	AviUtl::FileInfo m_fi = {};
	int32_t m_currentFrame = 0;
	ExEdit::Object* m_currentDrawObject = 0;
	FileCacheManager m_fileCacheManager;
	ItemCacheManager m_itemCacheManager;
	SubProcess m_subProcess;
	SubThreadManager m_subThreadManager;
	MainDialog m_mainDialog;

	int (*CallShowColorDialog)(DWORD u1, COLORREF* color, DWORD u3) = 0;

	BOOL m_exists = FALSE;
	int m_scale = 100;
	int m_showType = ShowType::Both;
	int m_updateMode = UpdateMode::On;
	int m_xorMode = XORMode::XOR;
	COLORREF m_penColor = RGB(0x00, 0xff, 0xff);
	COLORREF m_brushColor = RGB(0x00, 0xff, 0xff);
	BOOL m_showWaveform = TRUE;
	BOOL m_showText = TRUE;
	BOOL m_noScrollText = TRUE;
	BOOL m_behind = FALSE;

	App();
	~App();

	BOOL DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved);
	BOOL func_init(AviUtl::FilterPlugin* fp);
	BOOL func_exit(AviUtl::FilterPlugin* fp);
	BOOL func_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip);
	BOOL func_update(AviUtl::FilterPlugin* fp, AviUtl::FilterPlugin::UpdateStatus status);
	BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp);
	BOOL func_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size);
	BOOL func_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size);

	void load();
	void save();
	BOOL createDialog();
	BOOL updateProjectParams();
	BOOL updateItemCache(BOOL send);
	BOOL sendTotalsParams(TotalsParams* params); // サブプロセスに値を送る。
	TotalsParamsPtr receiveTotalsParams(); // 共有メモリから値を取得する。
	void drawWaveform(HDC dc, LPCRECT rcClip, LPCRECT rcItem);

	void setScale(int scale);
	void setShowType(int showType);
	void setUpdateMode(int updateMode);
	void setXORMode(int xorMode);
	void selectPenColor();
	void selectBrushColor();
	void clearAllCache();
	void showFull();
	void setShowWaveform(BOOL showWaveform);
	void setShowText(BOOL showText);
	void setNoScrollText(BOOL noScrollText);
	void setBehind(BOOL behind);
};

extern App theApp;

//--------------------------------------------------------------------
