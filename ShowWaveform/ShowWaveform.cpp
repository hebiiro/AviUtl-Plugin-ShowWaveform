#include "pch.h"
#include "ShowWaveform.h"
#include "App.h"

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

BOOL func_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size)
{
	return theApp.func_project_load(fp, editp, data, size);
}

BOOL func_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size)
{
	return theApp.func_project_save(fp, editp, data, size);
}

BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	return theApp.DllMain(instance, reason, reserved);
}

AviUtl::FilterPluginDLL* WINAPI GetFilterTable()
{
	LPCSTR name = "アイテム内音声波形";
	LPCSTR information = "アイテム内音声波形 7.3.1 by 蛇色";

	static AviUtl::FilterPluginDLL filter =
	{
		.flag =
			AviUtl::FilterPluginDLL::Flag::AlwaysActive |
//			AviUtl::FilterPluginDLL::Flag::DispFilter |
			AviUtl::FilterPluginDLL::Flag::WindowSize |
			AviUtl::FilterPluginDLL::Flag::WindowThickFrame |
			AviUtl::FilterPluginDLL::Flag::ExInformation,
		.x = 300,
		.y = 350,
		.name = name,
		.func_proc = func_proc,
		.func_init = func_init,
		.func_exit = func_exit,
		.func_update = func_update,
		.func_WndProc = func_WndProc,
		.information = information,
		.func_project_load = func_project_load,
		.func_project_save = func_project_save,
	};

	return &filter;
}
