////////////////////////////////////////////////////////////////////////////////
// yulib/extra.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef YULIB_EXTRA_H
#define YULIB_EXTRA_H

#include <stdio.h>
#include <windows.h>
#include <imagehlp.h>

#pragma comment(lib, "imagehlp.lib")

namespace yulib {\

////////////////////////////////////////////////////////////////////////////////
// 関数書き換え関数
// 
// 参考サイト
// http://qiita.com/kobake@github/items/8d3d3637c7af0b270098
////////////////////////////////////////////////////////////////////////////////

void* RewriteFunction(LPCSTR modname, LPCSTR funcname, void* funcptr)
{
	// ベースアドレス
	DWORD dwBase = (DWORD)GetModuleHandle(modname);
	if(!dwBase) return NULL;

	// イメージ列挙
	ULONG ulSize;
	PIMAGE_IMPORT_DESCRIPTOR pImgDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData((HMODULE)(intptr_t)dwBase, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
	for(; pImgDesc->Name; pImgDesc++){
		//const char* szModuleName = (char*)(intptr_t)(dwBase+pImgDesc->Name);
		// THUNK情報
		PIMAGE_THUNK_DATA pFirstThunk = (PIMAGE_THUNK_DATA)(intptr_t)(dwBase+pImgDesc->FirstThunk);
		PIMAGE_THUNK_DATA pOrgFirstThunk = (PIMAGE_THUNK_DATA)(intptr_t)(dwBase+pImgDesc->OriginalFirstThunk);
		// 関数列挙
		for(;pFirstThunk->u1.Function; pFirstThunk++, pOrgFirstThunk++){
			if(IMAGE_SNAP_BY_ORDINAL(pOrgFirstThunk->u1.Ordinal))continue;
			PIMAGE_IMPORT_BY_NAME pImportName = (PIMAGE_IMPORT_BY_NAME)(intptr_t)(dwBase+(DWORD)pOrgFirstThunk->u1.AddressOfData);

			// 書き換え判定
			if(_stricmp((const char*)pImportName->Name, funcname) != 0)continue;

			// 保護状態変更
			DWORD dwOldProtect;
			if( !VirtualProtect(&pFirstThunk->u1.Function, sizeof(pFirstThunk->u1.Function), PAGE_READWRITE, &dwOldProtect) )
				return NULL; // エラー

			// 書き換え
			void* pOrgFunc = (void*)(intptr_t)pFirstThunk->u1.Function; // 元のアドレスを保存しておく
			WriteProcessMemory(GetCurrentProcess(), &pFirstThunk->u1.Function, &funcptr, sizeof(pFirstThunk->u1.Function), NULL);
			pFirstThunk->u1.Function = (DWORD)(intptr_t)funcptr;

			// 保護状態戻し
			VirtualProtect(&pFirstThunk->u1.Function, sizeof(pFirstThunk->u1.Function), dwOldProtect, &dwOldProtect);
			return pOrgFunc; // 元のアドレスを返す
		}
	}
	return NULL;
}

} // namespace yulib

#endif // #ifndef YULIB_EXTRA_H