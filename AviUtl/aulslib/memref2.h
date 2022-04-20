/**
*	memref2.h by 蛇色
*
*	memref.h を改造して auls_memref.auf ではなく、
*	auls_memref.ini からアドレスを取得するようにした。
**/
#pragma once

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <strsafe.h>
#include <tchar.h>
typedef const BYTE* LPCBYTE;
#include "yulib/file.h"
#include "yulib/memory.h"
#include "aulslib/exedit.h"

struct lua_State;

namespace auls {

class CObjectBufferInfo;
class CUndoInfo;

class CMemref
{
public:

	UINT_PTR m_exedit;

	DWORD m_Exedit_StaticFilterTable;
	DWORD m_Exedit_SortedObjectTable_LayerIndexEnd;
	DWORD m_Exedit_AliasNameBuffer;
	DWORD m_Exedit_ObjDlg_CommandTarget;
	DWORD m_Exedit_SortedObjectTable_LayerIndexBegin;
	DWORD m_Exedit_ObjDlg_FilterStatus;
	DWORD m_Exedit_SortedObjectTable;
	DWORD m_Exedit_ObjDlg_ObjectIndex;
	DWORD m_Exedit_SceneSetting;
	DWORD m_Exedit_LoadedFilterTable;
	DWORD m_Exedit_LayerSetting;
	DWORD m_Exedit_SceneDisplaying;
	DWORD m_Exedit_TextBuffer;
	DWORD m_Exedit_TraScript_ProcessingTrackBarIndex;
	DWORD m_Exedit_TraScript_ProcessingObjectIndex;
	DWORD m_Exedit_ScriptProcessingFilter;
	DWORD m_Exedit_LuaState;
	DWORD m_Exedit_ObjectBufferInfo;
	DWORD m_Exedit_CameraZBuffer;
	DWORD m_Exedit_UndoInfo;

	DWORD m_Exedit_ObjectBufferInfo_exdata_size;
	DWORD m_Exedit_ObjectBufferInfo_max_data_num;
	DWORD m_Exedit_ObjectBufferInfo_data;
	DWORD m_Exedit_ObjectBufferInfo_exdata;

	DWORD m_Exedit_UndoInfo_current_id;
	DWORD m_Exedit_UndoInfo_write_offset;
	DWORD m_Exedit_UndoInfo_object_num;
	DWORD m_Exedit_UndoInfo_buffer_ptr;
	DWORD m_Exedit_UndoInfo_buffer_size;

public:

	CMemref()
	{
		m_exedit = 0;

		m_Exedit_StaticFilterTable = 0;
		m_Exedit_SortedObjectTable_LayerIndexEnd = 0;
		m_Exedit_AliasNameBuffer = 0;
		m_Exedit_ObjDlg_CommandTarget = 0;
		m_Exedit_SortedObjectTable_LayerIndexBegin = 0;
		m_Exedit_ObjDlg_FilterStatus = 0;
		m_Exedit_SortedObjectTable = 0;
		m_Exedit_ObjDlg_ObjectIndex = 0;
		m_Exedit_SceneSetting = 0;
		m_Exedit_LoadedFilterTable = 0;
		m_Exedit_LayerSetting = 0;
		m_Exedit_SceneDisplaying = 0;
		m_Exedit_TextBuffer = 0;
		m_Exedit_TraScript_ProcessingTrackBarIndex = 0;
		m_Exedit_TraScript_ProcessingObjectIndex = 0;
		m_Exedit_ScriptProcessingFilter = 0;
		m_Exedit_LuaState = 0;
		m_Exedit_ObjectBufferInfo = 0;
		m_Exedit_CameraZBuffer = 0;
		m_Exedit_UndoInfo = 0;

		m_Exedit_ObjectBufferInfo_exdata_size = 0;
		m_Exedit_ObjectBufferInfo_max_data_num = 0;
		m_Exedit_ObjectBufferInfo_data = 0;
		m_Exedit_ObjectBufferInfo_exdata = 0;

		m_Exedit_UndoInfo_current_id = 0;
		m_Exedit_UndoInfo_write_offset = 0;
		m_Exedit_UndoInfo_object_num = 0;
		m_Exedit_UndoInfo_buffer_ptr = 0;
		m_Exedit_UndoInfo_buffer_size = 0;
	}

	BOOL Init(FILTER *fp)
	{
		// 拡張編集のDLLハンドルを取得する。
		m_exedit = (UINT_PTR)::LoadLibraryEx(
			TEXT("exedit.auf"), 0, LOAD_WITH_ALTERED_SEARCH_PATH);

		if (!m_exedit)
			return FALSE; // 拡張編集のロードに失敗した。

		return loadAddress(fp);
	}

	BOOL loadAddress(FILTER *fp)
	{
		try
		{
			// iniFilePath にアドレスファイルのファイルパスを格納する。
			TCHAR iniFilePath[MAX_PATH] = {};
			::GetModuleFileName(fp->dll_hinst, iniFilePath, _countof(iniFilePath));
			::PathRemoveFileSpec(iniFilePath);
			::PathAppend(iniFilePath, TEXT("auls_memref.ini"));

			// appName に exedit.auf ファイルのハッシュ値を格納する。
			TCHAR appName[MAX_PATH] = {};
			{
				// exedit.auf ファイルのファイルパスを格納する。
				TCHAR path[MAX_PATH] = {};
				::GetModuleFileName((HMODULE)m_exedit, path, _countof(path));

				// ファイルを開く。
				yulib::CFile file; file.OpenExisting(path);

				// ファイルデータを取得する。
				DWORD fileSize = file.Size();
				yulib::CMemory<BYTE> fileData; fileData.Alloc(fileSize, true);
				BOOL result = file.Read(fileData, fileSize);

				if (!result)
					return FALSE; // ファイルデータの読み込みに失敗した。

				// ファイルデータから CRC32 を取得する。
				DWORD crc32 = yulib::Crc32(fileData, fileSize);

				::StringCbPrintf(appName, sizeof(appName), TEXT("%08X"), crc32);
			}

			// アドレスファイルからアドレスを取得する。
			// 読み込んだ数値が 0 の場合は例外が投げられる。

			m_Exedit_StaticFilterTable = getHex(iniFilePath, appName, TEXT("Exedit_StaticFilterTable"));
			m_Exedit_SortedObjectTable_LayerIndexEnd = getHex(iniFilePath, appName, TEXT("Exedit_SortedObjectTable_LayerIndexEnd"));
			m_Exedit_AliasNameBuffer = getHex(iniFilePath, appName, TEXT("Exedit_AliasNameBuffer"));
			m_Exedit_ObjDlg_CommandTarget = getHex(iniFilePath, appName, TEXT("Exedit_ObjDlg_CommandTarget"));
			m_Exedit_SortedObjectTable_LayerIndexBegin = getHex(iniFilePath, appName, TEXT("Exedit_SortedObjectTable_LayerIndexBegin"));
			m_Exedit_ObjDlg_FilterStatus = getHex(iniFilePath, appName, TEXT("Exedit_ObjDlg_FilterStatus"));
			m_Exedit_SortedObjectTable = getHex(iniFilePath, appName, TEXT("Exedit_SortedObjectTable"));
			m_Exedit_ObjDlg_ObjectIndex = getHex(iniFilePath, appName, TEXT("Exedit_ObjDlg_ObjectIndex"));
			m_Exedit_SceneSetting = getHex(iniFilePath, appName, TEXT("Exedit_SceneSetting"));
			m_Exedit_LoadedFilterTable = getHex(iniFilePath, appName, TEXT("Exedit_LoadedFilterTable"));
			m_Exedit_LayerSetting = getHex(iniFilePath, appName, TEXT("Exedit_LayerSetting"));
			m_Exedit_SceneDisplaying = getHex(iniFilePath, appName, TEXT("Exedit_SceneDisplaying"));
			m_Exedit_TextBuffer = getHex(iniFilePath, appName, TEXT("Exedit_TextBuffer"));
			m_Exedit_TraScript_ProcessingTrackBarIndex = getHex(iniFilePath, appName, TEXT("Exedit_TraScript_ProcessingTrackBarIndex"));
			m_Exedit_TraScript_ProcessingObjectIndex = getHex(iniFilePath, appName, TEXT("Exedit_TraScript_ProcessingObjectIndex"));
			m_Exedit_ScriptProcessingFilter = getHex(iniFilePath, appName, TEXT("Exedit_ScriptProcessingFilter"));
			m_Exedit_LuaState = getHex(iniFilePath, appName, TEXT("Exedit_LuaState"));
			m_Exedit_ObjectBufferInfo = getHex(iniFilePath, appName, TEXT("Exedit_ObjectBufferInfo"));
			m_Exedit_CameraZBuffer = getHex(iniFilePath, appName, TEXT("Exedit_CameraZBuffer"));
			m_Exedit_UndoInfo = getHex(iniFilePath, appName, TEXT("Exedit_UndoInfo"));

			m_Exedit_ObjectBufferInfo_exdata_size = getHex(iniFilePath, appName, TEXT("Exedit_ObjectBufferInfo_exdata_size"));
			m_Exedit_ObjectBufferInfo_max_data_num = getHex(iniFilePath, appName, TEXT("Exedit_ObjectBufferInfo_max_data_num"));
			m_Exedit_ObjectBufferInfo_data = getHex(iniFilePath, appName, TEXT("Exedit_ObjectBufferInfo_data"));
			m_Exedit_ObjectBufferInfo_exdata = getHex(iniFilePath, appName, TEXT("Exedit_ObjectBufferInfo_exdata"));

			m_Exedit_UndoInfo_current_id = getHex(iniFilePath, appName, TEXT("Exedit_UndoInfo_current_id"));
			m_Exedit_UndoInfo_write_offset = getHex(iniFilePath, appName, TEXT("Exedit_UndoInfo_write_offset"));
			m_Exedit_UndoInfo_object_num = getHex(iniFilePath, appName, TEXT("Exedit_UndoInfo_object_num"));
			m_Exedit_UndoInfo_buffer_ptr = getHex(iniFilePath, appName, TEXT("Exedit_UndoInfo_buffer_ptr"));
			m_Exedit_UndoInfo_buffer_size = getHex(iniFilePath, appName, TEXT("Exedit_UndoInfo_buffer_size"));

			return TRUE;
		}
		catch (...)
		{
		}

		return FALSE;
	}

	DWORD getHex(LPCTSTR path, LPCTSTR appName, LPCTSTR keyName)
	{
		TCHAR buffer[MAX_PATH];
		::GetPrivateProfileString(appName, keyName, TEXT(""), buffer, _countof(buffer), path);
		DWORD value = _tcstoul(buffer, 0, 16);
		if (!value) throw 0;
		return value;
	}

public:

	EXEDIT_FILTER** Exedit_StaticFilterTable()
	{
		return (EXEDIT_FILTER**)(m_exedit + m_Exedit_StaticFilterTable);
	}

	int* Exedit_SortedObjectTable_LayerIndexEnd()
	{
		return (int*)(m_exedit + m_Exedit_SortedObjectTable_LayerIndexEnd);
	}

	LPTSTR Exedit_AliasNameBuffer()
	{
		return (LPTSTR)(m_exedit + m_Exedit_AliasNameBuffer);
	}

	int Exedit_ObjDlg_CommandTarget()
	{
		return *(int*)(m_exedit + m_Exedit_ObjDlg_CommandTarget);
	}

	int* Exedit_SortedObjectTable_LayerIndexBegin()
	{
		return (int*)(m_exedit + m_Exedit_SortedObjectTable_LayerIndexBegin);
	}

	BYTE* Exedit_ObjDlg_FilterStatus()
	{
		return (BYTE*)(m_exedit + m_Exedit_ObjDlg_FilterStatus);
	}

	EXEDIT_OBJECT** Exedit_SortedObjectTable()
	{
		return (EXEDIT_OBJECT**)(m_exedit + m_Exedit_SortedObjectTable);
	}

	int Exedit_ObjDlg_ObjectIndex()
	{
		return *(int*)(m_exedit + m_Exedit_ObjDlg_ObjectIndex);
	}

	SCENE_SETTING* Exedit_SceneSetting()
	{
		return (SCENE_SETTING*)(m_exedit + m_Exedit_SceneSetting);
	}

	EXEDIT_FILTER** Exedit_LoadedFilterTable()
	{
		return (EXEDIT_FILTER**)(m_exedit + m_Exedit_LoadedFilterTable);
	}

	LAYER_SETTING* Exedit_LayerSetting()
	{
		return (LAYER_SETTING*)(m_exedit + m_Exedit_LayerSetting);
	}

	int Exedit_SceneDisplaying()
	{
		return *(int*)(m_exedit + m_Exedit_SceneDisplaying);
	}

	LPWSTR Exedit_TextBuffer()
	{
		return (LPWSTR)(m_exedit + m_Exedit_TextBuffer);
	}

	int Exedit_TraScript_ProcessingTrackBarIndex()
	{
		return *(int*)(m_exedit + m_Exedit_TraScript_ProcessingTrackBarIndex);
	}

	int Exedit_TraScript_ProcessingObjectIndex()
	{
		return *(int*)(m_exedit + m_Exedit_TraScript_ProcessingObjectIndex);
	}

	EXEDIT_FILTER* Exedit_ScriptProcessingFilter()
	{
		return (EXEDIT_FILTER*)(m_exedit + m_Exedit_ScriptProcessingFilter);
	}

	lua_State** Exedit_LuaState()
	{
		return (lua_State**)(m_exedit + m_Exedit_LuaState);
	}

	CObjectBufferInfo* Exedit_ObjectBufferInfo()
	{
		return (CObjectBufferInfo*)this;
	}

	CAMERA_ZBUFFER** Exedit_CameraZBuffer()
	{
		return (CAMERA_ZBUFFER**)(m_exedit + m_Exedit_CameraZBuffer);
	}

	CUndoInfo* Exedit_UndoInfo()
	{
		return (CUndoInfo*)this;
	}

public:

	DWORD Exedit_ObjectBufferInfo_exdata_size()
	{
		return *(DWORD*)(m_exedit + m_Exedit_ObjectBufferInfo_exdata_size);
	}

	DWORD Exedit_ObjectBufferInfo_max_data_num()
	{
		return *(DWORD*)(m_exedit + m_Exedit_ObjectBufferInfo_max_data_num);
	}

	EXEDIT_OBJECT* Exedit_ObjectBufferInfo_data()
	{
		return (EXEDIT_OBJECT*)(m_exedit + m_Exedit_ObjectBufferInfo_data);
	}

	void* Exedit_ObjectBufferInfo_exdata()
	{
		return *(void**)(m_exedit + m_Exedit_ObjectBufferInfo_exdata);
	}

public:

	int Exedit_UndoInfo_current_id()
	{
		return *(int*)(m_exedit + m_Exedit_UndoInfo_current_id);
	}

	DWORD Exedit_UndoInfo_write_offset()
	{
		return *(DWORD*)(m_exedit + m_Exedit_UndoInfo_write_offset);
	}

	int Exedit_UndoInfo_object_num()
	{
		return *(int*)(m_exedit + m_Exedit_UndoInfo_object_num);
	}

	void* Exedit_UndoInfo_buffer_ptr()
	{
		return *(void**)(m_exedit + m_Exedit_UndoInfo_buffer_ptr);
	}

	DWORD Exedit_UndoInfo_buffer_size()
	{
		return *(DWORD*)(m_exedit + m_Exedit_UndoInfo_buffer_size);
	}

};

class CObjectBufferInfo
{
public:

   __declspec(property(get = get_exdata_size)) DWORD exdata_size;
   __declspec(property(get = get_max_data_num)) DWORD max_data_num;
   __declspec(property(get = get_data)) EXEDIT_OBJECT* data;
   __declspec(property(get = get_exdata)) void* exdata;

public:

	DWORD get_exdata_size()
	{
		return ((CMemref*)this)->Exedit_ObjectBufferInfo_exdata_size();
	}

	DWORD get_max_data_num()
	{
		return ((CMemref*)this)->Exedit_ObjectBufferInfo_max_data_num();
	}

	EXEDIT_OBJECT* get_data()
	{
		return ((CMemref*)this)->Exedit_ObjectBufferInfo_data();
	}

	void* get_exdata()
	{
		return ((CMemref*)this)->Exedit_ObjectBufferInfo_exdata();
	}

};

class CUndoInfo
{
public:

   __declspec(property(get = get_current_id)) int current_id;
   __declspec(property(get = get_write_offset)) DWORD write_offset;
   __declspec(property(get = get_object_num)) int object_num;
   __declspec(property(get = get_buffer_ptr)) void* buffer_ptr;
   __declspec(property(get = get_buffer_size)) DWORD buffer_size;

public:

	int get_current_id()
	{
		return ((CMemref*)this)->Exedit_UndoInfo_current_id();
	}

	DWORD get_write_offset()
	{
		return ((CMemref*)this)->Exedit_UndoInfo_write_offset();
	}

	int get_object_num()
	{
		return ((CMemref*)this)->Exedit_UndoInfo_object_num();
	}

	void* get_buffer_ptr()
	{
		return ((CMemref*)this)->Exedit_UndoInfo_buffer_ptr();
	}

	DWORD get_buffer_size()
	{
		return ((CMemref*)this)->Exedit_UndoInfo_buffer_size();
	}

};

} // namespace auls
