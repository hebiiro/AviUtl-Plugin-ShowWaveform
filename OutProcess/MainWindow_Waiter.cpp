#include "pch.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

Waiter::Waiter(LPCSTR fileName)
	: fileName(fileName)
{
}

//--------------------------------------------------------------------

WaiterPtr MainWindow::createWaiter(LPCSTR fileName)
{
	MY_TRACE(_T("MainWindow::createWaiter(%hs)\n"), fileName);

	auto it = std::find_if(waiterQueue.begin(), waiterQueue.end(),
		[fileName](const WaiterPtr& waiter){ return waiter->fileName == fileName; });
	if (it != waiterQueue.end())
		return FALSE; // 既にキューに存在するので何もしない。

	WaiterPtr waiter = std::make_shared<Waiter>(fileName);
	waiterQueue.emplace_back(waiter);
	return waiter;
}

// ウェイターを消化する。
void MainWindow::digestWaiterQueue()
{
	MY_TRACE(_T("MainWindow::digestWaiterQueue()\n"));
#if 0
	// 無効なリーダープロセスを取り除く。
	for (auto it = readerMap.begin(); it != readerMap.end();)
	{
		DWORD code = 0;
		::GetExitCodeProcess(it->second->pi.hProcess, &code);
		if (code != STILL_ACTIVE)
		{
			MY_TRACE(_T("無効なリーダープロセスを削除します : %d, %d\n"),
				it->second->pi.dwProcessId, it->second->pi.dwThreadId);

			readerMap.erase(it++);
		}
		else
		{
			it++;
		}
	}
#endif

	// ウェイターが存在し、なおかつリーダーの数に空きがあるなら
	while (!waiterQueue.empty() && (int)readerMap.size() < maxReaderCount)
	{
		// ウェイターを取得する。
		const WaiterPtr& waiter = waiterQueue.front();

		// リーダーを作成する。
		ReaderPtr reader = createReader(waiter->fileName.c_str());

		// ウェイターを削除する。
		waiterQueue.pop_front();
	}

	MY_TRACE_INT(waiterQueue.size());
	MY_TRACE_INT(readerMap.size());
}

//--------------------------------------------------------------------
