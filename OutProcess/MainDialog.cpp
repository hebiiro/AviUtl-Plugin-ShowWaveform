#include "pch.h"
#include "MainDialog.h"
#include "App.h"

//--------------------------------------------------------------------

BOOL MainDialog::getShowBPM()
{
	HWND showBPM = ::GetDlgItem(*this, IDC_SHOW_BPM);
	return Button_GetCheck(showBPM) == BST_CHECKED;
}

BOOL MainDialog::setShowBPM(BOOL newShowBPM)
{
	HWND showBPM = ::GetDlgItem(*this, IDC_SHOW_BPM);
	Button_SetCheck(showBPM, newShowBPM ? BST_CHECKED : BST_UNCHECKED);
	return TRUE;
}

int MainDialog::getOrig()
{
	return ::GetDlgItemInt(*this, IDC_ORIG, 0, TRUE);
}

BOOL MainDialog::setOrig(int newOrig)
{
	return ::SetDlgItemInt(*this, IDC_ORIG, newOrig, TRUE);
}

int MainDialog::getBPM()
{
	return ::GetDlgItemInt(*this, IDC_BPM, 0, TRUE);
}

BOOL MainDialog::setBPM(int newBPM)
{
	return ::SetDlgItemInt(*this, IDC_BPM, newBPM, TRUE);
}

int MainDialog::getAbove()
{
	return ::GetDlgItemInt(*this, IDC_ABOVE, 0, TRUE);
}

BOOL MainDialog::setAbove(int newAbove)
{
	return ::SetDlgItemInt(*this, IDC_ABOVE, newAbove, TRUE);
}

int MainDialog::getBelow()
{
	return ::GetDlgItemInt(*this, IDC_BELOW, 0, TRUE);
}

BOOL MainDialog::setBelow(int newBelow)
{
	return ::SetDlgItemInt(*this, IDC_BELOW, newBelow, TRUE);
}

int MainDialog::getLimitVolume()
{
	return ::GetDlgItemInt(*this, IDC_LIMIT_VOLUME, 0, TRUE);
}

BOOL MainDialog::setLimitVolume(int newLimitVolume)
{
	return ::SetDlgItemInt(*this, IDC_LIMIT_VOLUME, newLimitVolume, TRUE);
}

int MainDialog::getBaseVolume()
{
	return ::GetDlgItemInt(*this, IDC_BASE_VOLUME, 0, TRUE);
}

BOOL MainDialog::setBaseVolume(int newBaseVolume)
{
	return ::SetDlgItemInt(*this, IDC_BASE_VOLUME, newBaseVolume, TRUE);
}

int MainDialog::getMinRMS()
{
	return ::GetDlgItemInt(*this, IDC_MIN_RMS, 0, TRUE);
}

BOOL MainDialog::setMinRMS(int newMinRMS)
{
	return ::SetDlgItemInt(*this, IDC_MIN_RMS, newMinRMS, TRUE);
}

int MainDialog::getMaxRMS()
{
	return ::GetDlgItemInt(*this, IDC_MAX_RMS, 0, TRUE);
}

BOOL MainDialog::setMaxRMS(int newMaxRMS)
{
	return ::SetDlgItemInt(*this, IDC_MAX_RMS, newMaxRMS, TRUE);
}

int MainDialog::getBaseRMS()
{
	return ::GetDlgItemInt(*this, IDC_BASE_RMS, 0, TRUE);
}

BOOL MainDialog::setBaseRMS(int newBaseRMS)
{
	return ::SetDlgItemInt(*this, IDC_BASE_RMS, newBaseRMS, TRUE);
}

int MainDialog::getZoom()
{
	return ::GetDlgItemInt(*this, IDC_ZOOM, 0, TRUE);
}

BOOL MainDialog::setZoom(int newZoom)
{
	return ::SetDlgItemInt(*this, IDC_ZOOM, newZoom, TRUE);
}

//--------------------------------------------------------------------

BOOL MainDialog::create(HINSTANCE instance, HWND parent)
{
	MY_TRACE(_T("MainDialog::create()\n"));

	// メインダイアログを作成する。

	if (!Dialog::create(instance, MAKEINTRESOURCE(IDD_CONFIG_DIALOG), parent))
	{
		::MessageBox(parent, _T("メインダイアログの作成に失敗しました"), _T("ShowWaveform"), MB_OK);

		return FALSE;
	}

	// コントロールを初期化する。

	ignoreNotification(TRUE);

	setShowBPM(theApp.getShowBPM());
	setOrig(theApp.getOrig());
	setBPM(theApp.getBPM());
	setAbove(theApp.getAbove());
	setBelow(theApp.getBelow());

	setLimitVolume(theApp.getLimitVolume());
	setBaseVolume(theApp.getBaseVolume());
	setMinRMS(theApp.getMinRMS());
	setMaxRMS(theApp.getMaxRMS());
	setBaseRMS(theApp.getBaseRMS());
	setZoom(theApp.getZoom());

	ignoreNotification(FALSE);

	return TRUE;
}

void MainDialog::ignoreNotification(BOOL ignoreNotification)
{
	m_ignoreNotification = ignoreNotification;
}

INT_PTR MainDialog::onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			MY_TRACE(_T("MainDialog::onDlgProc(WM_COMMAND, 0x%08X, 0x%08X)\n"), wParam, lParam);

			// App のセッターを使用するとダイアログが再更新されてしまうので
			// メインウィンドウのセッターを使用する。

			UINT code = HIWORD(wParam);
			UINT id = LOWORD(wParam);
			HWND sender = (HWND)lParam;

			switch (id)
			{
			case IDC_SHOW_BPM:
				{
					theApp.mainWindow.setShowBPM(getShowBPM());

					break;
				}
			case IDC_ORIG:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
						theApp.mainWindow.setOrig(getOrig());

					break;
				}
			case IDC_BPM:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
						theApp.mainWindow.setBPM(getBPM());

					break;
				}
			case IDC_ABOVE:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
						theApp.mainWindow.setAbove(getAbove());

					break;
				}
			case IDC_BELOW:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
						theApp.mainWindow.setBelow(getBelow());

					break;
				}
			case IDC_LIMIT_VOLUME:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
						theApp.mainWindow.setLimitVolume(getLimitVolume());

					break;
				}
			case IDC_BASE_VOLUME:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
						theApp.mainWindow.setBaseVolume(getBaseVolume());

					break;
				}
			case IDC_MIN_RMS:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
						theApp.mainWindow.setMinRMS(getMinRMS());

					break;
				}
			case IDC_MAX_RMS:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
						theApp.mainWindow.setMaxRMS(getMaxRMS());

					break;
				}
			case IDC_BASE_RMS:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
						theApp.mainWindow.setBaseRMS(getBaseRMS());

					break;
				}
			case IDC_ZOOM:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
						theApp.mainWindow.setZoom(getZoom());

					break;
				}
			}

			break;
		}
	case WM_NOTIFY:
		{
			MY_TRACE(_T("MainDialog::onDlgProc(WM_NOTIFY, 0x%08X, 0x%08X)\n"), wParam, lParam);

			NMHDR* header = (NMHDR*)lParam;
			if (header->code == UDN_DELTAPOS)
			{
				int value = ::GetDlgItemInt(hwnd, header->idFrom - 1, 0, TRUE);

				NM_UPDOWN* nm = (NM_UPDOWN*)header;
				if (nm->iDelta < 0)
				{
					value += 1;
				}
				else
				{
					value -= 1;
				}

				::SetDlgItemInt(hwnd, header->idFrom - 1, value, TRUE);
			}

			break;
		}
	}

	if (message == WM_AVIUTL_FILTER_RESIZE) return onAviUtlFilterResize(hwnd, message, wParam, lParam);

	return Dialog::onDlgProc(hwnd, message, wParam, lParam);
}

LRESULT MainDialog::onAviUtlFilterResize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("onAviUtlFilterResize(0x%08X, 0x%08X)\n"), wParam, lParam);

	HWND parent = ::GetParent(hwnd);
	RECT rc; ::GetClientRect(parent, &rc);
	MY_TRACE_RECT2(rc);
	::SetWindowPos(hwnd, 0, rc.left, rc.top, getWidth(rc), getHeight(rc), SWP_NOZORDER);

	return 0;
}

//--------------------------------------------------------------------
