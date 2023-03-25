#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
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
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <map>

#include "NanoVG/nanovg.h"
#pragma comment(lib, "NanoVG/NanoVGd32.lib")
#include "AviUtl/aviutl_exedit_sdk/aviutl.hpp"
#include "AviUtl/aviutl_exedit_sdk/exedit.hpp"
#include "Common/Tracer.h"
#include "Common/WinUtility.h"
#include "Common/Gdi.h"
#include "Common/Dialog.h"
#include "Common/Profile.h"
#include "Common/FileUpdateChecker.h"
#include "ShowWaveform/ShowWaveform/Share.h"
