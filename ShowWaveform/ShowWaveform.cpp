#include "pch.h"
#include "ShowWaveform.h"
#include "App.h"

//--------------------------------------------------------------------

// デバッグ用コールバック関数。デバッグメッセージを出力する。
void ___outputLog(LPCTSTR text, LPCTSTR output)
{
	::OutputDebugString(output);
}

//--------------------------------------------------------------------

int TRACK_SCALE = 0;
int TRACK_SHOW_TYPE = 1;
int TRACK_UPDATE_MODE = 2;

LPCSTR track_name[] =
{
	"描画サイズ",
	"表示タイプ",
	"更新モード",
};
int track_def[] = {  200, 1, 1 };
int track_min[] = {    1, 0, 0 };
int track_max[] = { 2000, 2, 1 };

int CHECK_CLEAR = 0;
int CHECK_SHOW_WAVEFORM = 1;
int CHECK_SHOW_TEXT = 2;
int CHECK_NO_SCROLL_TEXT = 3;

LPCSTR check_name[] =
{
	"キャッシュをクリア",
	"音声波形を表示",
	"テキストを表示",
	"テキストをスクロールしない",
};
int check_def[] = { -1, 1, 1, 1 };

//--------------------------------------------------------------------

BOOL func_init(AviUtl::FilterPlugin* fp)
{
	return theApp.func_init(fp);
}

BOOL func_exit(AviUtl::FilterPlugin* fp)
{
	return theApp.func_exit(fp);
}

BOOL func_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
{
	return theApp.func_proc(fp, fpip);
}

BOOL func_update(AviUtl::FilterPlugin* fp, AviUtl::FilterPlugin::UpdateStatus status)
{
	return theApp.func_update(fp, status);
}

BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
{
	return theApp.func_WndProc(hwnd, message, wParam, lParam, editp, fp);
}

BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	return theApp.DllMain(instance, reason, reserved);
}

//--------------------------------------------------------------------

AviUtl::FilterPluginDLL* WINAPI GetFilterTable()
{
	theApp.load(track_def, check_def);

	LPCSTR name = "アイテム内音声波形";
	LPCSTR information = "アイテム内音声波形 3.0.0 by 蛇色";

	static AviUtl::FilterPluginDLL filter =
	{
		.flag =
			AviUtl::FilterPluginDLL::Flag::AlwaysActive |
			AviUtl::FilterPluginDLL::Flag::DispFilter |
			AviUtl::FilterPluginDLL::Flag::ExInformation,
		.name = name,
		.track_n = sizeof(track_name) / sizeof(*track_name),
		.track_name = track_name,
		.track_default = track_def,
		.track_s = track_min,
		.track_e = track_max,
		.check_n = sizeof(check_name) / sizeof(*check_name),
		.check_name = check_name,
		.check_default = check_def,
		.func_proc = func_proc,
		.func_init = func_init,
		.func_exit = func_exit,
		.func_update = func_update,
		.func_WndProc = func_WndProc,
		.information = information,
	};

	return &filter;
}

//--------------------------------------------------------------------
