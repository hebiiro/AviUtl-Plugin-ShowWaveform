#pragma once

#include "Share.h"

//--------------------------------------------------------------------

struct Sender
{
	// ���̓f�[�^�B
	char fileName[MAX_PATH];
	int32_t video_scale;
	int32_t video_rate;

	// ���C���X���b�h���̏����B
	Sender(LPCSTR fileName);
};

//--------------------------------------------------------------------
