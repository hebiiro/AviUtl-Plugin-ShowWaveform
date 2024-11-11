#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include <commdlg.h>
#pragma comment(lib, "comdlg32.lib")
#include <mmsystem.h>
#include <ks.h>
#include <ksmedia.h>
#pragma comment(lib, "winmm.lib")
#include <glad/gl.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#include <strsafe.h>
#include <comdef.h>

#include <tchar.h>
#include <clocale>
#include <cmath>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <deque>

#include "NanoVG/nanovg.h"
#pragma comment(lib, "NanoVG/NanoVGd32.lib")
#include "aviutl.hpp"
#include "exedit.hpp"
#include "Common/Tracer.h"
#include "Common/WinUtility.h"
#include "Common/Gdi.h"
#include "Common/Dialog.h"
#include "Common/Profile.h"
#include "Common/FileUpdateChecker.h"
#include "../ShowWaveform/Share.h"

//#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
//#endif
