#pragma once

#include "Resource.h"

//--------------------------------------------------------------------

struct MainDialog : public Dialog
{
	BOOL m_ignoreNotification = FALSE;

	void ignoreNotification(BOOL ignoreNotification);
	virtual INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

//--------------------------------------------------------------------
