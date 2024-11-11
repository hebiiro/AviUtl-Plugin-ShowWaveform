#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#pragma comment(lib, "msimg32.lib")
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include <comdef.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <strsafe.h>

#include <tchar.h>
#include <algorithm>
#include <numeric>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <istream>

#include "aviutl.hpp"
#include "exedit.hpp"
#include "Common/Tracer.h"
#include "Common/WinUtility.h"
#include "Common/Dialog.h"
#include "Common/Profile.h"
#include "Common/Hook.h"
#include "Common/AviUtlInternal.h"
#include "detours/detours.h"
#pragma comment(lib, "detours/detours.lib")
