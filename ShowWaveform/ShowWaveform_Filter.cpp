#include "pch.h"
#include "ShowWaveform.h"

//---------------------------------------------------------------------

int TRACK_FRAME_INC = 0;
int TRACK_SAMPLE_INC = 1;
int TRACK_SCALE_DIV = 2;
int TRACK_SHOW_TYPE = 3;
int TRACK_UPDATE_MODE = 4;

LPCSTR track_name[] =
{
	"フレーム増加数",
	"サンプル増加数",
	"縮小率",
	"表示タイプ",
	"更新モード",
};
int track_def[] = {  10,  10,  80, 0, 0 };
int track_min[] = {   1,   1,   1, 0, 0 };
int track_max[] = { 600, 800, 400, 2, 2 };

int CHECK_UPDATE_SELECTED_ITEM = 0;
int CHECK_UPDATE_ALL_ITEMS = 1;
int CHECK_DELETE_SELECTED_ITEM = 2;
int CHECK_DELETE_ALL_ITEMS = 3;
int CHECK_SHOW_WAVEFORM = 4;
int CHECK_SHOW_TEXT = 5;

LPCSTR check_name[] =
{
	"選択アイテムを更新",
	"すべてのアイテムを更新",
	"選択アイテムの波形を消去",
	"すべてのアイテムの波形を消去",
	"音声波形を表示",
	"テキストを表示",
};
int check_def[] = { -1, -1, -1, -1, 1, 1 };

//---------------------------------------------------------------------
//		フィルタ構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTable(void)
{
	theApp.load(track_def);

	static TCHAR filterName[] = TEXT("アイテム内音声波形");
	static TCHAR filterInformation[] = TEXT("アイテム内音声波形 version 2.0.0 by 蛇色");

	static FILTER_DLL filter =
	{
//		FILTER_FLAG_NO_CONFIG | // このフラグを指定するとウィンドウが作成されなくなってしまう。
		FILTER_FLAG_ALWAYS_ACTIVE | // このフラグがないと「フィルタ」に ON/OFF を切り替えるための項目が追加されてしまう。
//		FILTER_FLAG_DISP_FILTER | // このフラグがないと「設定」の方にウィンドウを表示するための項目が追加されてしまう。
		FILTER_FLAG_AUDIO_FILTER |
//		FILTER_FLAG_WINDOW_THICKFRAME |
//		FILTER_FLAG_WINDOW_SIZE |
		FILTER_FLAG_EX_INFORMATION,
		0, 0,
		filterName,
		sizeof(track_name) / sizeof(*track_name), (TCHAR**)track_name, track_def, track_min, track_max,
		sizeof(check_name) / sizeof(*check_name), (TCHAR**)check_name, check_def,
		func_proc,
		func_init,
		func_exit,
		func_update,
		func_WndProc,
		NULL, NULL,
		NULL,
		NULL,
		filterInformation,
		NULL, NULL,
		NULL, NULL, NULL, NULL,
		NULL,
	};

	return &filter;
}

//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------

BOOL func_init(FILTER *fp)
{
	return theApp.func_init(fp);
}

//---------------------------------------------------------------------
//		終了
//---------------------------------------------------------------------
BOOL func_exit(FILTER *fp)
{
	return theApp.func_exit(fp);
}

//---------------------------------------------------------------------
//		フィルタ関数
//---------------------------------------------------------------------
BOOL func_proc(FILTER *fp, FILTER_PROC_INFO *fpip)
{
	return theApp.func_proc(fp, fpip);
}

//---------------------------------------------------------------------
//		更新
//---------------------------------------------------------------------

BOOL func_update(FILTER *fp, int status)
{
	return theApp.func_update(fp, status);
}

//---------------------------------------------------------------------
//		WndProc
//---------------------------------------------------------------------
BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, void *editp, FILTER *fp)
{
	return theApp.func_WndProc(hwnd, message, wParam, lParam, editp, fp);
}

//---------------------------------------------------------------------
//		DllMain
//---------------------------------------------------------------------
BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	return theApp.DllMain(instance, reason, reserved);
}

//---------------------------------------------------------------------
