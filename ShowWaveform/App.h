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
	BOOL m_exists = FALSE;

	AviUtlInternal m_auin;

	HINSTANCE m_instance = 0;
	AviUtl::FilterPlugin* m_fp = 0;
	AviUtl::FileInfo m_fi = {};
	ExEdit::Object* m_currentDrawObject = 0;
	FileCacheManager m_fileCacheManager;
	ItemCacheManager m_itemCacheManager;
	SubProcess m_subProcess;
	SubThreadManager m_subThreadManager;

	int (*CallShowColorDialog)(DWORD u1, COLORREF* color, DWORD u3) = 0;

	int m_scale = 100;
	struct ShowType {
		static const int Both = 0;
		static const int Min = 1;
		static const int Max = 2;
	};
	int m_showType = ShowType::Both;
	struct UpdateMode {
		static const int Off = 0;
		static const int On = 1;
	};
	int m_updateMode = UpdateMode::On;
	struct XORMode {
		static const int None = 0;
		static const int XOR = 1;
		static const int NotXOR = 2;
		static const int Not = 3;
	};
	int m_xorMode = XORMode::XOR;
	COLORREF m_penColor = RGB(0x00, 0xff, 0xff);
	COLORREF m_brushColor = RGB(0x00, 0xff, 0xff);
	BOOL m_showWaveform = TRUE;
	BOOL m_showText = TRUE;
	BOOL m_noScrollText = TRUE;

	struct ControlID {
		struct Edit {
			static const UINT scale			= 1000;
		};
		struct Spin {
			static const UINT scale			= 2000;
		};
		struct ComboBox {
			static const UINT showType		= 1100;
			static const UINT updateMode	= 1101;
			static const UINT xorMode		= 1102;
		};
		struct Button {
			static const UINT penColor		= 1200;
			static const UINT brushColor	= 1201;
			static const UINT clear			= 1202;
			static const UINT showFull		= 1203;
		};
		struct CheckBox {
			static const UINT showWaveform	= 1300;
			static const UINT showText		= 1301;
			static const UINT noScrollText	= 1302;
		};
	};

	struct {
		struct {
			HWND scale = 0;
			HWND showType = 0;
			HWND updateMode = 0;
			HWND xorMode = 0;
		} label;
		struct {
			HWND scale = 0;
		} edit;
		struct {
			HWND scale = 0;
		} spin;
		struct {
			HWND showType = 0;
			HWND updateMode = 0;
			HWND xorMode = 0;
		} comboBox;
		struct {
			HWND penColor = 0;
			HWND brushColor = 0;
			HWND clear = 0;
			HWND showFull = 0;
		} button;
		struct {
			HWND showWaveform = 0;
			HWND showText = 0;
			HWND noScrollText = 0;
		} checkBox;
	} controls;

	App();
	~App();

	BOOL DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved);
	void createControls();
	void recalcLayout();
	void load();
	void save();
	BOOL func_init(AviUtl::FilterPlugin* fp);
	BOOL func_exit(AviUtl::FilterPlugin* fp);
	BOOL func_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip);
	BOOL func_update(AviUtl::FilterPlugin* fp, AviUtl::FilterPlugin::UpdateStatus status);
	BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp);

	BOOL updateItemCache(BOOL send);
	void drawWaveform(HDC dc, LPCRECT rcClip, LPCRECT rcItem);
};

extern App theApp;

//---------------------------------------------------------------------
