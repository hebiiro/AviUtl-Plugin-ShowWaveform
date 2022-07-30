#include "pch.h"
#include "ShowWaveform.h"

//--------------------------------------------------------------------

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
int track_def[] = {  10,  10,   80, 0, 0 };
int track_min[] = {   1,   1,    1, 0, 0 };
int track_max[] = { 600, 800, 4000, 2, 2 };

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

EXTERN_C AviUtl::FilterPluginDLL* CALLBACK GetFilterTable()
{
	theApp.load(track_def);

	LPCSTR name = "アイテム内音声波形";
	LPCSTR information = "アイテム内音声波形 2.2.0 by 蛇色";

	static AviUtl::FilterPluginDLL filter =
	{
		.flag =
			AviUtl::detail::FilterPluginFlag::AlwaysActive |
			AviUtl::detail::FilterPluginFlag::DispFilter |
			AviUtl::detail::FilterPluginFlag::ExInformation,
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
