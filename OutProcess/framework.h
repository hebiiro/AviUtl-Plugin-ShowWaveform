#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <mmsystem.h>
#include <ks.h>
#include <ksmedia.h>
#pragma comment(lib, "winmm.lib")
#include <strsafe.h>
#include <comdef.h>

#include <tchar.h>
#include <clocale>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <map>

#include "AviUtl/aviutl_exedit_sdk/aviutl.hpp"
#include "AviUtl/aviutl_exedit_sdk/exedit.hpp"
#include "Common/Tracer.h"
#include "Common/WinUtility.h"
#include "ShowWaveform/ShowWaveform/Share.h"
