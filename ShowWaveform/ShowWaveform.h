#pragma once

//---------------------------------------------------------------------
// Define

#define DECLARE_INTERNAL_PROC(resultType, callType, procName, args) \
	typedef resultType (callType *Type_##procName) args; \
	Type_##procName procName = NULL

#define DECLARE_HOOK_PROC(resultType, callType, procName, args) \
	typedef resultType (callType *Type_##procName) args; \
	extern Type_##procName true_##procName; \
	resultType callType hook_##procName args

#define IMPLEMENT_HOOK_PROC(resultType, callType, procName, args) \
	Type_##procName true_##procName = procName; \
	resultType callType hook_##procName args

#define IMPLEMENT_HOOK_PROC_NULL(resultType, callType, procName, args) \
	Type_##procName true_##procName = NULL; \
	resultType callType hook_##procName args

#define GET_INTERNAL_PROC(module, procName) \
	procName = (Type_##procName)::GetProcAddress(module, #procName)

#define GET_HOOK_PROC(module, procName) \
	true_##procName = (Type_##procName)::GetProcAddress(module, #procName)

#define ATTACH_HOOK_PROC(name) DetourAttach((PVOID*)&true_##name, hook_##name)

//---------------------------------------------------------------------
// Api Hook

typedef __int64 (*Type_FrameToX)(int frame);

DECLARE_HOOK_PROC(void, CDECL, DrawObject, (HDC dc, int objectIndex));

BOOL WINAPI drawObjectText(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx);

//---------------------------------------------------------------------
// Function

// CALL を書き換える
template<class T>
void hookCall(DWORD address, T hookProc)
{
	BYTE code[5];
	code[0] = 0xE8; // CALL
	*(DWORD*)&code[1] = (DWORD)hookProc - (address + 5);

	// CALL を書き換える。そのあと命令キャッシュをフラッシュする。
	::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, code, sizeof(code), NULL);
	::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, sizeof(code));
}

// CALL を書き換える
template<class T>
void hookAbsoluteCall(DWORD address, T& hookProc)
{
	BYTE code[6];
	code[0] = 0xE8; // CALL
	*(DWORD*)&code[1] = (DWORD)hookProc - (address + 5);
	code[5] = 0x90; // NOP

	// CALL を書き換える。そのあと命令キャッシュをフラッシュする。
	::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, code, sizeof(code), NULL);
	::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, sizeof(code));
}

// 絶対アドレスを書き換える
template<class T>
void writeAbsoluteAddress(DWORD address, const T* x)
{
	// 絶対アドレスを書き換える。そのあと命令キャッシュをフラッシュする。
	::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, &x, sizeof(x), NULL);
	::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, sizeof(x));
}

//---------------------------------------------------------------------

extern int TRACK_FRAME_INC;
extern int TRACK_SAMPLE_INC;
extern int TRACK_SCALE_DIV;
extern int TRACK_SHOW_TYPE;

extern int CHECK_UPDATE_SELECTED_ITEM;
extern int CHECK_UPDATE_ALL_ITEMS;
extern int CHECK_DELETE_SELECTED_ITEM;
extern int CHECK_DELETE_ALL_ITEMS;
extern int CHECK_SHOW_WAVEFORM;
extern int CHECK_SHOW_TEXT;
extern int CHECK_UPDATE_ALWAYS;

//---------------------------------------------------------------------

class CShowWaveformApp
{
public:

	struct Sample
	{
		Sample(int frame, short min, short max)
			: m_frame(frame)
			, m_min(min)
			, m_max(max)
		{
		}

		int m_frame;
		short m_min;
		short m_max;
	};

	typedef std::vector<Sample> SampleArray;

	struct Waveform
	{
		SampleArray m_sampleArray;
		auls::EXEDIT_OBJECT m_objectCopy = {};
		int m_frameBegin = 0;
		int m_frameEnd = 0;
	};

	typedef std::shared_ptr<Waveform> WaveformPtr;
	typedef std::map<auls::EXEDIT_OBJECT*, WaveformPtr> WaveformMap;

	struct GetWaveform
	{
		DWORD startTime = 0;
		DWORD endTime = 0;

		FILTER* filter = 0;
		void* editp = 0;
		FILE_INFO fi = {};
		std::vector<short> buffer;
		FILTER_PROC_INFO fpi = {};
		int frameInc = 0;
		int sampleInc = 0;

		GetWaveform(FILTER *fp);
		~GetWaveform();
		void get(auls::EXEDIT_OBJECT* object);
		void getInternal(auls::EXEDIT_OBJECT* object, Waveform* waveform, int frame);
	};

public:

	HINSTANCE m_instance;
	FILTER* m_fp;
	auls::EXEDIT_OBJECT* m_currentDrawObject;
	WaveformMap m_waveformMap;

	Type_FrameToX m_FrameToX;

	HWND* g_exeditWindow = 0; // 拡張編集ウィンドウ。
	HWND* g_settingDialog = 0; // 設定ダイアログ。
	auls::EXEDIT_OBJECT** g_objectTable = 0;
	auls::EXEDIT_FILTER** g_filterTable = 0;
	int* g_objectIndex = 0; // カレントオブジェクトのインデックスへのポインタ。
	int* g_filterIndex = 0; // カレントフィルタのインデックスへのポインタ。
	int* g_objectCount = 0; // オブジェクトの数。
	int* g_currentSceneObjectCount = 0; // カレントシーンのオブジェクトの数。
	auls::EXEDIT_OBJECT** g_objectData = 0; // オブジェクトデータへのポインタ。
	BYTE** g_objectExdata = 0; // オブジェクト拡張データへのポインタ。
	int* g_nextObject = 0; // 次のオブジェクトの配列へのポインタ。
	void** g_editp = 0; // editp へのポインタ。

	COLORREF m_penColor;
	COLORREF m_brushColor;

	HWND Exedit_GetExeditWindow()
	{
		return *g_exeditWindow;
	}

	HWND Exedit_GetSettingDialog()
	{
		return *g_settingDialog;
	}

	int Exedit_GetCurrentObjectIndex()
	{
		return *g_objectIndex;
	}

	int Exedit_GetCurrentFilterIndex()
	{
		return *g_filterIndex;
	}

	int Exedit_GetObjectCount()
	{
		return *g_objectCount;
	}

	int Exedit_GetCurrentSceneObjectCount()
	{
		return *g_currentSceneObjectCount;
	}

	auls::EXEDIT_OBJECT* Exedit_GetObject(int objectIndex)
	{
		return *g_objectData + objectIndex;
	}

	auls::EXEDIT_OBJECT* Exedit_GetSortedObject(int objectIndex)
	{
		return g_objectTable[objectIndex];
	}

	auls::EXEDIT_FILTER* Exedit_GetFilter(int filterId)
	{
		return g_filterTable[filterId];
	}

	auls::EXEDIT_FILTER* Exedit_GetFilter(auls::EXEDIT_OBJECT* object, int filterIndex)
	{
		if (!object) return 0;
		int id = object->filter_param[filterIndex].id;
		if (id < 0) return 0;
		return Exedit_GetFilter(id);
	}

	int Exedit_GetNextObjectIndex(int objectIndex)
	{
		return g_nextObject[objectIndex];
	}

	void* Exedit_GetEditp()
	{
		return *g_editp;
	}

public:

	CShowWaveformApp();
	~CShowWaveformApp();

	BOOL DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved);
	void load(int* track_def);
	BOOL func_init(FILTER *fp);
	BOOL func_exit(FILTER *fp);
	BOOL func_proc(FILTER *fp, FILTER_PROC_INFO *fpip);
	BOOL func_update(FILTER *fp, int status);
	BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, void *editp, FILTER *fp);

	void getWaveform(FILTER *fp);
	void getAllWaveform(FILTER *fp);
	void deleteWaveform(FILTER *fp);
	void deleteAllWaveform(FILTER *fp);
	void drawWaveform(HDC dc, LPCRECT rc);
};

extern CShowWaveformApp theApp;

//---------------------------------------------------------------------
