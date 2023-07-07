#pragma once

#include "OutProcess.h"
#include "ReaderManager.h"

//--------------------------------------------------------------------

struct Waiter {
	std::string fileName;
	Waiter(LPCSTR fileName);
};

typedef std::shared_ptr<Waiter> WaiterPtr;
typedef std::deque<WaiterPtr> WaiterQueue;

//--------------------------------------------------------------------

struct WaiterManager {
	WaiterQueue waiterQueue;
	WaiterPtr createWaiter(LPCSTR fileName);
	void digestWaiterQueue(ReaderManager& readerManager);
};

//--------------------------------------------------------------------
