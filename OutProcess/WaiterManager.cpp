#include "pch.h"
#include "WaiterManager.h"

//--------------------------------------------------------------------

Waiter::Waiter(LPCSTR fileName)
	: fileName(fileName)
{
}

//--------------------------------------------------------------------

WaiterPtr WaiterManager::createWaiter(LPCSTR fileName)
{
	MY_TRACE(_T("WaiterManager::createWaiter(%hs)\n"), fileName);

	auto it = std::find_if(waiterQueue.begin(), waiterQueue.end(),
		[fileName](const WaiterPtr& waiter){ return waiter->fileName == fileName; });
	if (it != waiterQueue.end())
		return FALSE; // 既にキューに存在するので何もしない。

	WaiterPtr waiter = std::make_shared<Waiter>(fileName);
	waiterQueue.emplace_back(waiter);
	return waiter;
}

// ウェイターを消化する。
void WaiterManager::digestWaiterQueue(ReaderManager& readerManager)
{
	MY_TRACE(_T("WaiterManager::digestWaiterQueue()\n"));

	// ウェイターが存在し、なおかつリーダーの数に空きがあるなら
	while (!waiterQueue.empty() && readerManager.hasEmpty())
	{
		// ウェイターを取得する。
		const WaiterPtr& waiter = waiterQueue.front();

		// リーダーを作成する。
		ReaderPtr reader = readerManager.createReader(waiter->fileName.c_str());

		// ウェイターを削除する。
		waiterQueue.pop_front();
	}

	MY_TRACE_INT(waiterQueue.size());
	MY_TRACE_INT(readerManager.readerMap.size());
}

//--------------------------------------------------------------------
