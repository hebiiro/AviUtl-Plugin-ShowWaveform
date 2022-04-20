////////////////////////////////////////////////////////////////////////////////
// aulslib/exedit.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef AULSLIB_EXEDIT_H
#define AULSLIB_EXEDIT_H

#include <yulib/color.h>

#include <aulslib/aviutl.h>

namespace auls {\

static const LPCSTR EXEDIT_NAME = "拡張編集";
static const LPCSTR OBJDLG_CLASSNAME = "ExtendedFilterClass";

static const int EXEDIT_FILTER_ID_BEGIN = 2000;
static const int EXEDIT_ALIAS_ID_BEGIN  = 3000;

// 設定ダイアログコマンドID
struct OBJDLG_COMMAND {
	static const DWORD FILTER_SET_DEFVALUE = 1108;
	static const DWORD FILTER_MAKE_ALIAS   = 1109;
	static const DWORD FILTER_MOVEUP   = 1116;
	static const DWORD FILTER_MOVEDOWN = 1117;
	static const DWORD FILTER_DELETE   = 4300;
	static const DWORD FILTER_INIT     = 1105;
	static const DWORD FILTER_VALIDATE = 4400;
	static const DWORD FILTER_FOLD     = 4500;
};

inline FILTER *Exedit_GetFilter(FILTER *fp)
{
	for(int i = AviUtl_GetFilterNumber(fp); i;) {
		FILTER *exedit = (FILTER*)fp->exfunc->get_filterp(--i);
		if(!strcmp(exedit->name, EXEDIT_NAME)) return exedit;
	}
	return NULL;
}

inline HWND Exedit_GetWindow(FILTER *fp)
{
	FILTER *exedit = Exedit_GetFilter(fp);
	return exedit ? exedit->hwnd : NULL;
}

// オブジェクト設定ウィンドウを取得
//  ただし当該ウィンドウが作成されるのが WM_FILTER_INIT 時点なので、
//  プラグインの処理順序の都合、確実に取得できるのは WM_FILTER_CHANGE_WINDOW 以後である。
inline HWND ObjDlg_GetWindow(HWND hwnd_exedit)
{
	struct local {
		HWND exedit;
		HWND objdlg;

		static BOOL CALLBACK enumproc(HWND hwnd, LPARAM lp) {
			local *p = (local*)lp;
			HWND owner = GetWindow(hwnd, GW_OWNER);
			if(owner == p->exedit) {
				p->objdlg = hwnd;
				return FALSE;
			} else {
				return TRUE;
			}
		}
	} l = { hwnd_exedit, NULL };

	DWORD id = GetCurrentThreadId();
	EnumThreadWindows(id, local::enumproc, (LPARAM)&l);
	return l.objdlg;
}



struct EXEDIT_OBJECT {
	// 定数宣言
	static const BYTE FLAG_CLIPPING = 0x01;
	static const BYTE FLAG_CAMERA   = 0x02;

	/*
	static const BYTE TYPE_IMAGE_FILTER = 0;
	static const BYTE TYPE_IMAGE_OBJECT = 1;
	static const BYTE TYPE_SOUND_FILTER = 2;
	static const BYTE TYPE_SOUND_OBJECT = 3;
	static const BYTE TYPE_IMAGE_EFFECT = 4;
	static const BYTE TYPE_SOUND_EFFECT = 6; // 通常、単体で配置はできない
	static const BYTE TYPE_CAMERA_TIME_CONTROL  = 0x18;
	static const BYTE TYPE_GROUP_CONTROL = 0x1C;*/
	static const BYTE TYPE_OVERLAY   = 0x01; 
	static const BYTE TYPE_SOUND     = 0x02;
	static const BYTE TYPE_EFFECT    = 0x04;
	static const BYTE TYPE_LIGHTBLUE   = 0x08; // グループ制御、カメラ制御(カメラエフェクト含む)、時間制御
	static const BYTE TYPE_CONTROL = 0x10; // 対象レイヤー範囲がタイムライン上に表示される

	static const size_t MAX_DISPNAME = 64;
	static const int    MAX_FILTER = 12;
	static const int    MAX_TRACK = 64;
	static const int    MAX_CHECK = 48;

	static const BYTE FILTER_STATUS_VALID = 0x01;
	static const BYTE FILTER_STATUS_FOLD = 0x02;
	static const BYTE FILTER_STATUS_GUI_VALID = 0x04;

	struct FILTER_PARAM {
		static const int INVALID_ID = -1;

		int   id;
		short track_begin; // このフィルタの先頭のトラックバー番号
		short check_begin;
		DWORD exdata_offset;
	};

	struct TRACK_MODE {
		static const short NUM_USE_SCRIPT = 0x0F;
		static const short NUM_DECELERATE = 0x20;
		static const short NUM_ACCELERATE = 0x40;

		short num;
		short script_num;

		bool Accelerate(void) { return (num & NUM_ACCELERATE) != 0; }
		bool Decelerate(void) { return (num & NUM_DECELERATE) != 0; }
		bool UseScript(void)  { return (num & NUM_USE_SCRIPT) == NUM_USE_SCRIPT; }
	};

	// 変数宣言
	BYTE exists;
	BYTE flag; // 1:上のオブジェクトでクリッピング 2:カメラ制御の対象
	BYTE type; // 
	BYTE padding_;
	int  layer_disp; // 表示レイヤー、別シーン表示中は-1
	int  frame_begin;
	int  frame_end;
	char dispname[MAX_DISPNAME]; // タイムライン上の表示名(最初のバイトがヌル文字の場合はデフォルト名が表示)
	int  index_midpt_leader; // 中間点を持つオブジェクトの構成要素の場合、先頭オブジェクトのインデックス、中間点を持たないなら-1
	FILTER_PARAM filter_param[MAX_FILTER];
	BYTE filter_status[MAX_FILTER];
	short track_sum;
	short check_sum;
	DWORD exdata_sum;
	int   track_value_left[MAX_TRACK];
	int   track_value_right[MAX_TRACK];
	TRACK_MODE track_mode[MAX_TRACK];
	int   check_value[MAX_CHECK];
	DWORD exdata_offset;
	int   group_belong;
	int   track_param[MAX_TRACK];
	int   layer_set;
	int   scene_set;

	// 関数定義
	int GetFilterNum(void)
	{
		for(int i = 0; i < MAX_FILTER; i++) {
			if(filter_param[i].id == FILTER_PARAM::INVALID_ID) return i;
		}
		return MAX_FILTER;
	}

	DWORD ExdataOffset(int idx)
	{
		return exdata_offset + filter_param[idx].exdata_offset;
	}
};

struct EXEDIT_FILTER {
	static const DWORD FLAG_INPUT_FILTER   = 0x00000008;
	static const DWORD FLAG_OUTPUT_FILTER  = 0x00000010;
	static const DWORD FLAG_EFFECT         = 0x00000020;
	static const DWORD FLAG_DISABLE_PUT    = 0x00000080; // タイムライン配置メニューには出てこない
	static const DWORD FLAG_DISABLE_ADDING = 0x00000100; // タイムライン配置メニューにのみ出現、エフェクト追加メニューには出てこない
	static const DWORD FLAG_UNKNOWN1       = 0x00000200; // これを外すとマスク、ディスプレイスメントマップでシーンが使えなくなる。拡張色設定で色が変化しなくなる。
	static const DWORD FLAG_BASIC_EFFECT   = 0x00008000;
	static const DWORD FLAG_SOUND_FILTER   = 0x00200000;
	static const DWORD FLAG_CONTROL_FILTER = 0x01000000;
	static const DWORD FLAG_UNKNOWN_RUNTIME = 0x04000000; // 実行時にすべてのフィルタに設定されている(初期化完了フラグ？)
	static const DWORD FLAG_UNKNOWN        = 0x40000000;

	// 拡張データ項目(ボタンやドロップダウンリスト等)のデータ
	struct EXDATA_USE {
		static const WORD TYPE_UNKNOWN = 0; // name は NULL
		static const WORD TYPE_NUMBER  = 1;
		static const WORD TYPE_STRING  = 2;
		static const WORD TYPE_BYTEARRAY = 3;

		WORD type;
		WORD size;
		LPCSTR name; // 項目名
	};

	struct TRACK_EXTRA {
		int *track_scale;
		int *track_link;
		int *track_drag_min;
		int *track_drag_max;
	};

	// メインウィンドウの点線枠の表示に必要な項目に対応しているトラックバー番号
	struct TRACK_GUI {
		static const int INVALID = -1;

		int bx, by, bz; // 基準座標
		int rx, ry, rz; // 回転
		int cx, cy, cz; // 中心座標
		int zoom, aspect, alpha;
	};

	DWORD flag;
	int   x; // 0x10000=出力フィルタなら変更時にカメラ制御対象フラグがon(グループ制御にもついている)
	int   y; // デフォルト長さ(正ならフレーム数、負ならミリ秒、ゼロならタイムライン拡大率に応じた長さ)
	LPCSTR name;
	int    track_num; // 0x10
	LPCSTR *track_name;
	int    *track_def;
	int    *track_min;
	int    *track_max; // 0x20
	int     check_num;
	LPCSTR *check_name;
	int    *check_def; // -1:ボタン化 -2:ドロップダウンリスト化
	void   *func_proc; // 0x30
	void   *func_init;
	void   *func_exit;
	void   *func_update;
	void   *func_WndProc; // 0x40
	void   *track_value;
	int    *check_value;
	void   *exdata_ptr;
	DWORD   exdata_size; // 0x50
	void   *information; // 常に NULL
	void   *func_save_start;
	void   *func_save_end;
	EXFUNC *aviutl_exfunc; // 0x60
	void   *exedit_exfunc;
	void   *dll_inst; // 常にNULL
	void   *exdata_def; // 0x6C : 拡張データの初期値
	EXDATA_USE *exdata_use; // 拡張データ項目の名前に関するデータ
	TRACK_EXTRA *track_extra; // track_scale, ?, track_drag_min, track_drag_max
	TRACK_GUI   *track_gui;
	int     unknown[20];
	int    *track_scale; // 0xCC : 小数点第1位までなら10、第2位までなら100、小数点無しなら0
	void   *track_link;
	int    *track_drag_min;
	int    *track_drag_max;
	FILTER *exedit_filter;  // 拡張編集のフィルタ構造体
	EXEDIT_OBJECT *object_data; // 0xE0
	short  object_index_processing; // 処理中オブジェクトのインデックス
	short  filter_pos_processing; // 処理中オブジェクトのフィルタ位置
	short  object_index_objdlg; // 上と同じ？
	short  filter_pos_objdlg;
	int    frame_start; // オブジェクトの開始フレーム
	int    frame_end;   // 0xF0 : オブジェクトの終了フレーム
	int    *track_value_left; // 
	int    *track_value_right;
	int    *track_mode;
	int    *check_value_; // 0x100
	void   *exdata_;
	int    *track_param;
	void   *offset_10C;
	void   *offset_110;
	void   *offset_114;
	int     frame_start_chain; // 0x118 : 中間点オブジェクト全体の開始フレーム
	int     frame_end_chain; // 0x11C 同終了フレーム
	int     layer_set; // 配置レイヤー
	int     scene_set; // 配置シーン

	bool IsScreenFilter(void)
	{
		return (flag & (FLAG_INPUT_FILTER | FLAG_OUTPUT_FILTER | FLAG_EFFECT | FLAG_CONTROL_FILTER)) == 0;
	}

	int GetExdataUseNum(void)
	{
		if(exdata_size == 0) return 0;
		int num = 0;
		DWORD sum = 0;
		while(sum < exdata_size) {
			sum += exdata_use[num].size;
			num++;
		}
		return num;
	}
};

struct OBJECT_BUFFER_INFO {
	DWORD exdata_size;
	DWORD max_data_num;
	EXEDIT_OBJECT *data;
	void* exdata;
};

struct LAYER_SETTING {
	// 定数
	static const DWORD FLAG_UNDISP = 1;
	static const DWORD FLAG_LOCKED = 2;
	static const DWORD FLAG_COORDLINK = 0x10;
	static const DWORD FLAG_CLIP = 0x20;

	// 変数
	DWORD flag;
	LPCSTR name;
};

struct SCENE_SETTING {
	static const DWORD FLAG_DISPED = 1;
	static const DWORD FLAG_ALPHA  = 2;

	DWORD flag;
	LPCSTR name;
	int width;
	int height;
	int max_frame;
	int current_frame;
	int timeline_scale; // 
	int timeline_disp_begin_pos;
	int selected_object;
	int selected_frame_begin;
	int selected_frame_end;
	BOOL disp_bpm_grid;
	int bpm_grid_tempo; // 表示を10000倍した値
	int bpm_grid_base;
	BOOL disp_xy_grid;
	int xy_grid_width;
	int xy_grid_height;
	BOOL disp_camera_grid;
	int camera_grid_size;
	int camera_grid_num;
	BOOL disp_out_of_frame;
	int out_of_frame_scale; // フレーム外領域の拡大率、12で等倍。大きいほど縮小される
	int bpm_grid_beat;
	int disp_begin_layer;
};

struct CAMERA_ZBUFFER {
	static const DWORD DISTANCE_BASE = 0x77359400; // カメラとの距離が 0 の時の値。1 離れると 0x1000 減る
	static const DWORD ROTATE_MAX = 0x4000;

	DWORD distance;
	DWORD rotate; // XY軸回転をかけると変化する。 0x4000 が最大？
	DWORD distance_overlapped;
	yulib::ColorYCbCrA color;

	int GetDistance(void)
	{
		return (DISTANCE_BASE - distance) / 0x1000;
	}

	int GetDistanceOverlapped(void)
	{
		return (DISTANCE_BASE - distance) / 0x1000;
	}
};

struct UNDO_INFO {
	int   object_num;
	void* buffer_ptr;
	DWORD write_offset;
	int   current_id;
	DWORD buffer_size;
};

struct UNDO_DATA_HEADER {
	int   data_id;   // アンドゥデータの識別子。この値が最大のものから実行される。
	int   object_id; // 対象オブジェクトのID
	DWORD data_size; // ヘッダ部も含んだデータサイズ
	void  *data;

	UNDO_DATA_HEADER *Next(void) { return (UNDO_DATA_HEADER*)((DWORD)this + data_size); }
};

struct CACHE_BUFFER {
	static const DWORD FLAG_IMAGE_FILE = 0x20; // 画像ファイル、obj.copybuffer("image:xxxx")
	static const DWORD FLAG_IMAGE_COPY = 0x40; // モーションブラー、obj.copybuffer("cache:xxxx")

	int width;
	int height;
	DWORD flag;
	yulib::ColorYCbCrA *data;
	char path[260]; // obj.copybuffer に "cache:xxxx" という名前を渡した場合
	                // *lua_xxxx として格納される
};




} // namespace auls

#endif // #ifndef AULSLIB_EXEDIT_H