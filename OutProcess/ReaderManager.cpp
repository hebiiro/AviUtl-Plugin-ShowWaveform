#include "pch.h"
#include "ReaderManager.h"
#include "App.h"

//--------------------------------------------------------------------

Reader::Reader(HWND hwnd)
{
	MY_TRACE(_T("Reader::Reader(0x%08p)\n"), hwnd);

	TCHAR path[MAX_PATH] = {};
	::GetModuleFileName(theApp.instance, path, MAX_PATH);
	::PathRemoveFileSpec(path);
	::PathAppend(path, _T("ReaderProcess.exe"));
	MY_TRACE_TSTR(path);

	TCHAR args[MAX_PATH] = {};
	::StringCbPrintf(args, sizeof(args), _T("0x%08p"), hwnd);
	MY_TRACE_TSTR(args);

	STARTUPINFO si = { sizeof(si) };
	if (!::CreateProcess(
		path,           // No module name (use command line)
		args,           // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi))           // Pointer to PROCESS_INFORMATION structure
	{
		MY_TRACE(_T("::CreateProcess() failed.\n"));

		return;
	}

	shared.init(getSharedReaderBottleName(getId()));
}

Reader::~Reader()
{
	MY_TRACE(_T("Reader::~Reader() : %d, %d\n"), pi.dwProcessId, pi.dwThreadId);

	::CloseHandle(pi.hThread);
	::CloseHandle(pi.hProcess);
}

DWORD Reader::getId()
{
	return pi.dwThreadId;
}

ReaderBottle* Reader::getBottle()
{
	return shared.getBuffer();
}

//--------------------------------------------------------------------

// リーダーを取得する。
ReaderPtr ReaderManager::getReader(DWORD id)
{
	auto it = readerMap.find(id);
	if (it == readerMap.end()) return 0;
	return it->second;
}

// リーダーを作成する。
ReaderPtr ReaderManager::createReader(LPCSTR fileName)
{
	MY_TRACE(_T("ReaderManager::createReader(%hs)\n"), fileName);

	ReaderPtr reader = std::make_shared<Reader>(theApp.mainWindow);
	ReaderBottle* shared = reader->getBottle();
	if (!shared) return 0;
	::StringCbCopyA(shared->fileName, sizeof(shared->fileName), fileName);
	readerMap[reader->getId()] = reader;
	return reader;
}

// リーダーを削除する。
void ReaderManager::eraseReader(DWORD id)
{
	MY_TRACE(_T("ReaderManager::eraseReader(%d)\n"), id);

	readerMap.erase(id);
}

BOOL ReaderManager::hasEmpty()
{
	return (int)readerMap.size() < maxReaderCount;
}

//--------------------------------------------------------------------
