#include "pch.h"
#include "SubProcess.h"
#include "App.h"

//--------------------------------------------------------------------

BOOL SubProcess::init(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubProcess::init()\n"));

	TCHAR path[MAX_PATH] = {};
	::GetModuleFileName(fp->dll_hinst, path, MAX_PATH);
	::PathRemoveExtension(path);
	::PathAppend(path, _T("ShowWaveform.exe"));
	MY_TRACE_TSTR(path);

	TCHAR args[MAX_PATH] = {};
	::StringCbPrintf(args, sizeof(args), _T("0x%08p"), fp->hwnd);
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
		&m_pi))         // Pointer to PROCESS_INFORMATION structur
	{
		MY_TRACE(_T("::CreateProcess() failed.\n"));

		return FALSE;
	}

	return TRUE;
}

BOOL SubProcess::exit(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("SubProcess::exit()\n"));

	return ::PostMessage(m_mainWindow, WM_AVIUTL_FILTER_EXIT, 0, 0);
}

//--------------------------------------------------------------------
