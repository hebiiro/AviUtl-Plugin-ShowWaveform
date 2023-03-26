#include "pch.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

Reader::Reader(HWND hwnd)
{
	MY_TRACE(_T("Reader::Reader(0x%08p)\n"), hwnd);

	TCHAR path[MAX_PATH] = {};
	::GetModuleFileName(g_instance, path, MAX_PATH);
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
ReaderPtr MainWindow::getReader(DWORD id)
{
	auto it = readerMap.find(id);
	if (it == readerMap.end()) return 0;
	return it->second;
}

// リーダーを作成する。
ReaderPtr MainWindow::createReader(LPCSTR fileName)
{
	MY_TRACE(_T("MainWindow::createReader(%hs)\n"), fileName);

	ReaderPtr reader = std::make_shared<Reader>(m_hwnd);
	ReaderBottle* shared = reader->getBottle();
	if (!shared) return 0;
	::StringCbCopyA(shared->fileName, sizeof(shared->fileName), fileName);
	readerMap[reader->getId()] = reader;
	return reader;
}

// リーダーを削除する。
void MainWindow::eraseReader(DWORD id)
{
	MY_TRACE(_T("MainWindow::eraseReader(%d)\n"), id);

	readerMap.erase(id);
}

// リーダーがボトルに詰め込んだ音声サンプルからキャッシュを作成する。
CachePtr MainWindow::createCache(const ReaderPtr& reader)
{
	MY_TRACE(_T("MainWindow::createCache()\n"));

	ReaderBottle* shared = reader->getBottle();
	if (!shared) return 0;

	MY_TRACE_STR(shared->fileName);

	CachePtr cache = std::make_shared<Cache>();
	cache->fileName = shared->fileName;
	cache->samples.insert(cache->samples.end(),
		shared->samples, shared->samples + shared->sampleCount);
	cacheMap[cache->fileName] = cache;
	return cache;
}

// キャッシュをボトルに詰めてからメインプロセスに送る。(受け取るように促す)
void MainWindow::sendCache(const CachePtr& cache)
{
	MY_TRACE(_T("MainWindow::sendCache(%hs)\n"), cache->fileName.c_str());

	ReceiverBottle* shared = m_sharedReceiverBottle.getBuffer();
	MY_TRACE_HEX(shared);
	if (!shared) return;

	::StringCbCopyA(shared->fileName, sizeof(shared->fileName), cache->fileName.c_str());

	shared->sampleCount = (int32_t)cache->samples.size();
	MY_TRACE_INT(shared->sampleCount);
	memcpy(shared->samples, cache->samples.data(), sizeof(Sample) * cache->samples.size());

	MY_TRACE_HEX(g_parent);
	::SendMessage(g_parent, WM_AVIUTL_FILTER_RECEIVE, 0, 0);
}

//--------------------------------------------------------------------
