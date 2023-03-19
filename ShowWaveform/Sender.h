#pragma once

#include "Share.h"

//--------------------------------------------------------------------

struct Sender
{
	// 入力データ。
	char fileName[MAX_PATH];
	int32_t video_scale;
	int32_t video_rate;

	// メインスレッド側の処理。
	Sender(LPCSTR fileName);
};

//--------------------------------------------------------------------
