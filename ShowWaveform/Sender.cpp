#include "pch.h"
#include "Sender.h"
#include "App.h"

//--------------------------------------------------------------------

Sender::Sender(LPCSTR fileName)
{
	::StringCbCopyA(this->fileName, sizeof(this->fileName), fileName);
	this->video_scale = theApp.m_fi.video_scale;
	this->video_rate = theApp.m_fi.video_rate;
}

//--------------------------------------------------------------------
