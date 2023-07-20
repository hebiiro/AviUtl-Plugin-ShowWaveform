#include "pch.h"
#include "App.h"
#include "Common/Tracer2.h"

//--------------------------------------------------------------------

App theApp;
Shared shared;

//--------------------------------------------------------------------

BOOL App::getShowBPM()
{
	return mainWindow.getShowBPM();
}

BOOL App::setShowBPM(BOOL newShowBPM)
{
	if (!mainWindow.setShowBPM(newShowBPM))
		return FALSE;

	mainDialog.ignoreNotification(TRUE);
	mainDialog.setShowBPM(mainWindow.getShowBPM());
	mainDialog.ignoreNotification(FALSE);

	return TRUE;
}

int App::getOrig()
{
	return mainWindow.getOrig();
}

BOOL App::setOrig(int newOrig)
{
	if (!mainWindow.setOrig(newOrig))
		return FALSE;

	mainDialog.ignoreNotification(TRUE);
	mainDialog.setOrig(mainWindow.getOrig());
	mainDialog.ignoreNotification(FALSE);

	return TRUE;
}

int App::getBPM()
{
	return mainWindow.getBPM();
}

BOOL App::setBPM(int newBPM)
{
	if (!mainWindow.setBPM(newBPM))
		return FALSE;

	mainDialog.ignoreNotification(TRUE);
	mainDialog.setBPM(mainWindow.getBPM());
	mainDialog.ignoreNotification(FALSE);

	return TRUE;
}

int App::getAbove()
{
	return mainWindow.getAbove();
}

BOOL App::setAbove(int newAbove)
{
	if (!mainWindow.setAbove(newAbove))
		return FALSE;

	mainDialog.ignoreNotification(TRUE);
	mainDialog.setAbove(mainWindow.getAbove());
	mainDialog.ignoreNotification(FALSE);

	return TRUE;
}

int App::getBelow()
{
	return mainWindow.getBelow();
}

BOOL App::setBelow(int newBelow)
{
	if (!mainWindow.setBelow(newBelow))
		return FALSE;

	mainDialog.ignoreNotification(TRUE);
	mainDialog.setBelow(mainWindow.getBelow());
	mainDialog.ignoreNotification(FALSE);

	return TRUE;
}

int App::getLimitVolume()
{
	return mainWindow.getLimitVolume();
}

BOOL App::setLimitVolume(int newLimitVolume)
{
	if (!mainWindow.setLimitVolume(newLimitVolume))
		return FALSE;

	mainDialog.ignoreNotification(TRUE);
	mainDialog.setLimitVolume(mainWindow.getLimitVolume());
	mainDialog.ignoreNotification(FALSE);

	return TRUE;
}

int App::getBaseVolume()
{
	return mainWindow.getBaseVolume();
}

BOOL App::setBaseVolume(int newBaseVolume)
{
	if (!mainWindow.setBaseVolume(newBaseVolume))
		return FALSE;

	mainDialog.ignoreNotification(TRUE);
	mainDialog.setBaseVolume(mainWindow.getBaseVolume());
	mainDialog.ignoreNotification(FALSE);

	return TRUE;
}

int App::getMinRMS()
{
	return mainWindow.getMinRMS();
}

BOOL App::setMinRMS(int newMinRMS)
{
	if (!mainWindow.setMinRMS(newMinRMS))
		return FALSE;

	mainDialog.ignoreNotification(TRUE);
	mainDialog.setMinRMS(mainWindow.getMinRMS());
	mainDialog.ignoreNotification(FALSE);

	return TRUE;
}

int App::getMaxRMS()
{
	return mainWindow.getMaxRMS();
}

BOOL App::setMaxRMS(int newMaxRMS)
{
	if (!mainWindow.setMaxRMS(newMaxRMS))
		return FALSE;

	mainDialog.ignoreNotification(TRUE);
	mainDialog.setMaxRMS(mainWindow.getMaxRMS());
	mainDialog.ignoreNotification(FALSE);

	return TRUE;
}

int App::getBaseRMS()
{
	return mainWindow.getBaseRMS();
}

BOOL App::setBaseRMS(int newBaseRMS)
{
	if (!mainWindow.setBaseRMS(newBaseRMS))
		return FALSE;

	mainDialog.ignoreNotification(TRUE);
	mainDialog.setBaseRMS(mainWindow.getBaseRMS());
	mainDialog.ignoreNotification(FALSE);

	return TRUE;
}

int App::getZoom()
{
	return mainWindow.getZoom();
}

BOOL App::setZoom(int newZoom)
{
	if (!mainWindow.setZoom(newZoom))
		return FALSE;

	mainDialog.ignoreNotification(TRUE);
	mainDialog.setZoom(mainWindow.getZoom());
	mainDialog.ignoreNotification(FALSE);

	return TRUE;
}

//--------------------------------------------------------------------

void CALLBACK App::timerProc(HWND hwnd, UINT message, UINT_PTR timerId, DWORD time)
{
	if (!::IsWindow(theApp.windowContainer))
	{
		MY_TRACE(_T("コンテナウィンドウが無効になりました\n"));

		::PostQuitMessage(0);
	}
}

int APIENTRY App::WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	struct Initializer
	{
		Initializer()
		{
			_tsetlocale(LC_ALL, _T(""));
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

	windowContainer = (HWND)_tcstoull(::GetCommandLine(), 0, 0);
	MY_TRACE_HEX(windowContainer);

	dialogContainer = (HWND)::GetProp(windowContainer, PROP_NAME_DIALOG_CONTAINER);
	MY_TRACE_HEX(dialogContainer);

	if (!shared.init(windowContainer))
	{
		::MessageBox(windowContainer, _T("共有メモリの初期化に失敗しました"), _T("ShowWaveform.OutProcess"), MB_OK);
		return -1;
	}

	if (!mainWindow.create(instance, windowContainer))
	{
		::MessageBox(windowContainer, _T("メインウィンドウの作成に失敗しました"), _T("ShowWaveform.OutProcess"), MB_OK);
		return -1;
	}

	MY_TRACE_HWND((HWND)mainWindow);

	if (!mainDialog.create(instance, dialogContainer))
	{
		::MessageBox(windowContainer, _T("メインダイアログの作成に失敗しました"), _T("ShowWaveform.OutProcess"), MB_OK);
		return -1;
	}

	MY_TRACE_HWND((HWND)mainDialog);

	// InProcess にウィンドウハンドルを渡す。
	::PostMessage(windowContainer, WM_AVIUTL_FILTER_INIT, (WPARAM)(HWND)mainWindow, 0);
	::PostMessage(dialogContainer, WM_AVIUTL_FILTER_INIT, (WPARAM)(HWND)mainDialog, 0);

	// InProcess を監視するタイマーを作成する。
	// メインウィンドウは削除されるかもしれないので WM_TIMER は使用できない。
	::SetTimer(0, 0, 1000, timerProc);

	MSG msg = {};
	while (::GetMessage(&msg, 0, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	shared.term();

	MY_TRACE(_T("プロセスが正常終了しました\n"));

	return 0;
}

LRESULT App::onAviUtlFilterSend(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterSend(0x%08X, 0x%08X)\n"), wParam, lParam);

	switch (wParam)
	{
	case SendID::requestCache:
		{
			SenderBottlePtr bottle = shared.getSenderBottle();
			if (bottle)
			{
				CachePtr cache = cacheManager.getCache(bottle->fileName);
				if (cache)
				{
					// キャッシュが作成済みならメインプロセスに通知する。
					sendCache(cache);
				}
				else
				{
					// キャッシュが存在しないので新規作成する。
					// (1) ファイル名からウェイターを作成する。
					// (2) ウェイターからリーダーを作成する。
					// (3) リーダーからキャッシュを作成する。

					// ウェイターを作成する。
					WaiterPtr waiter = waiterManager.createWaiter(bottle->fileName);

					// 可能であれば、ウェイターを消化する。
					waiterManager.digestWaiterQueue(readerManager);
				}
			}

			break;
		}
	case SendID::notifyProjectChanged:
		{
			ProjectParamsPtr params = shared.getProjectParams();
			if (params)
				setProjectParams(params);

			break;
		}
	case SendID::notifyItemChanged:
		{
			AudioParamsPtr params = shared.getAudioParams();
			if (params)
				setAudioParams(params);

			break;
		}
	case SendID::notifyTotalsChanged:
		{
			TotalsParamsPtr params = shared.getSenderTotalsParams();
			if (params)
				setTotalsParams(params);

			break;
		}
	}

	return 0;
}

LRESULT App::onAviUtlFilterReceive(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterReceive(0x%08X, 0x%08X)\n"), wParam, lParam);

	DWORD id = (DWORD)wParam;
	MY_TRACE_INT(id);

	// リーダーを取得する。
	ReaderPtr reader = readerManager.getReader(id);
	if (reader)
	{
		// キャッシュを作成する。
		CachePtr cache = cacheManager.createCache(reader);
		if (cache)
		{
			// キャッシュを作成できた場合はメインプロセスに通知する。
			sendCache(cache);

			// このリーダーは不要になったので削除する。
			readerManager.eraseReader(id);

			// リーダーの空きができたので次の読み込みを開始する。
			waiterManager.digestWaiterQueue(readerManager);
		}
	}

	return 0;
}

LRESULT App::onAviUtlFilterClear(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterClear(0x%08X, 0x%08X)\n"), wParam, lParam);

	cacheManager.clear();

	return 0;
}

LRESULT App::onAviUtlFilterRedraw(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterRedraw(0x%08X, 0x%08X)\n"), wParam, lParam);

	if (::IsWindowVisible(mainWindow))
	{
		// 全体の音声波形を再計算する。
		recalcTotals();
	}

	return 0;
}

//--------------------------------------------------------------------

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	return theApp.WinMain(instance, prevInstance, cmdLine, cmdShow);
}

//--------------------------------------------------------------------
