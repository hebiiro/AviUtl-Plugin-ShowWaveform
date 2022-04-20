////////////////////////////////////////////////////////////////////////////////
// aulslib/aviutl.h
// AviUtl本体およびプラグイン用ヘッダ
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef AULSLIB_AVIUTL_H
#define AULSLIB_AVIUTL_H

#include <stdio.h>
#include <windows.h>

#ifndef AVIUTL_PLUGIN_SDK_FILTER_H
#define AVIUTL_PLUGIN_SDK_FILTER_H
#include <aviutl_plugin_sdk/filter.h>
#endif // #ifndef AVIUTL_PLUGIN_SDK_FILTER_H

namespace auls {\

static const LPCSTR AVIUTL_EXFUNC_NAME[] = {
	"get_ycp_ofs",
	"get_ycp",
	"get_pixelp",
	"get_audio",
	"is_editing",
	"is_saving",
	"get_frame",
	"get_frame_n",
	"get_frame_size",
	"set_frame",
	"set_frame_n",
	"copy_frame",
	"copy_video",
	"copy_audio",
	"copy_clip",
	"paste_clip",
	"get_frame_status",
	"set_frame_status",
	"is_saveframe",
	"is_keyframe",
	"is_recompress",
	"filter_window_update",
	"is_filter_window_disp",
	"get_file_info",
	"get_config_name",
	"is_filter_active",
	"get_pixel_filtered",
	"get_audio_filtered",
	"get_select_frame",
	"set_select_frame",
	"rgb2yc",
	"yc2rgb",
	"dlg_get_load_name",
	"dlg_get_save_name",
	"ini_load_int",
	"ini_save_int",
	"ini_load_str",
	"ini_save_str",
	"get_source_file_info",
	"get_source_video_number",
	"get_sys_info",
	"get_filterp",
	"get_ycp_filtering",
	"get_audio_filtering",
	"set_ycp_filtering_cache_size",
	"get_ycp_filtering_cache",
	"get_ycp_source_cache",
	"get_disp_pixelp",
	"get_pixel_source",
	"get_pixel_filtered_ex",
	"get_ycp_filtering_cache_ex",
	"exec_multi_thread_func",
	"create_yc",
	"delete_yc",
	"load_image",
	"resize_yc",
	"copy_yc",
	"draw_text",
	"avi_file_open",
	"avi_file_close",
	"avi_file_read_video",
	"avi_file_read_audio",
	"avi_file_get_video_pixelp",
	"get_avi_file_filter",
	"avi_file_read_audio_sample",
	"avi_file_set_audio_sample_rate",
	"get_frame_status_table",
	"set_undo",
	"add_menu_item",
	"edit_open",
	"edit_close",
	"edit_output",
	"set_config",
	"", "", "", "", "", "", "",
};

inline int AviUtl_GetVersionNumber(FILTER *fp)
{
	SYS_INFO si;
	fp->exfunc->get_sys_info(NULL, &si);
	return si.build;
}

inline int AviUtl_GetFilterNumber(FILTER *fp)
{
	SYS_INFO si;
	fp->exfunc->get_sys_info(NULL, &si);
	return si.filter_n;
}

inline HFONT AviUtl_GetFilterWindowFont(FILTER *fp)
{
	SYS_INFO si;
	fp->exfunc->get_sys_info(NULL, &si);
	return si.hfont;
}

inline FILTER* AviUtl_GetFilter(FILTER *fp, LPCSTR name)
{
	for(int i = AviUtl_GetFilterNumber(fp); i;) {
		FILTER *temp = (FILTER*)fp->exfunc->get_filterp(--i);
		if(!strcmp(temp->name, name)) return temp;
	}
	return NULL;
}

inline HWND AviUtl_GetWindow(FILTER *fp)
{
	if(fp->hwnd) {
		return GetWindow(fp->hwnd, GW_OWNER);
	} else {
		for(int i = 0; FILTER *temp = (FILTER*)fp->exfunc->get_filterp(i); i++) {
			if(temp->hwnd) return GetWindow(temp->hwnd, GW_OWNER);
		}
	}
	return NULL;
}

struct AVIUTL_SYSTEM_SETTING {
};

struct EDIT_HANDLE {
	DWORD flag;
	char edit_filename[260];
	char output_filename[260];
	char project_filename[260];
};

// FILTER構造体のラッパクラス
//  fp->の形でFILTER構造体のメンバと追加関数にアクセスできる
class CFilterWrapper {
public:
	class wrap : public FILTER {
	public:
		int SetTrackValue(int n, int val, bool do_update)
		{
			if(n < 0 || n >= this->track_n) return 0;
			if(val < this->track_s[n]) this->track[n] = this->track_s[n];
			else if(val > this->track_e[n]) this->track[n] = this->track_e[n];
			else this->track[n] = val;
			if(do_update) this->exfunc->filter_window_update(this);
			return this->track[n];
		}

		int AddTrackValue(int n, int val, bool do_update)
		{
			if(n < 0 || n >= this->track_n) return 0;
			return SetTrackValue(n, this->track[n] + val, do_update);
		}

		bool SetCheckValue(int n, bool val, bool do_update)
		{
			if(n < 0 || n >= this->check_n) return false;
			this->check[n] = val ? 1 : 0;
			if(do_update) this->exfunc->filter_window_update(this);
			return this->check[n] != 0;
		}

		bool ReverseCheckValue(int n, bool do_update)
		{
			if(n < 0 || n >= this->check_n) return false;
			return SetCheckValue(n, !this->check[n], do_update);
		}
	} *fp;

	CFilterWrapper(FILTER *fp_) : fp((wrap*)fp_) {}
	wrap* operator->() { return fp; }
};

} // namespace auls

#endif // #ifndef AULSLIB_AVIUTL_H
