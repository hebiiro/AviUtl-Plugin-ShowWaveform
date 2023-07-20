#include "pch.h"
#include "App.h"
#include "Common/Tracer2.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	struct Initializer
	{
		Initializer()
		{
			_tsetlocale(LC_CTYPE, _T(""));
			trace_init(0, 0, TRUE);
			::OleInitialize(0);
		}

		~Initializer()
		{
			trace_term();
			::OleUninitialize();
		}

	} initializer;

	MY_TRACE(_T("WinMain()\n"));

	app.init(instance);
	app.receive();
	app.send();
	app.term();

	MY_TRACE(_T("プロセスが正常終了しました\n"));

	return 0;
}
