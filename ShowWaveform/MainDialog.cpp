#include "pch.h"
#include "MainDialog.h"
#include "App.h"

//---------------------------------------------------------------------

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

			UINT code = HIWORD(wParam);
			UINT id = LOWORD(wParam);
			HWND sender = (HWND)lParam;

			switch (id)
			{
			case IDC_SCALE:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
					{
						int scale = (int)::GetDlgItemInt(hwnd, IDC_SCALE, 0, TRUE);
						theApp.setScale(scale);
					}

					break;
				}
			case IDC_SHOW_TYPE:
				{
					if (code == CBN_SELCHANGE)
					{
						HWND comboBox = ::GetDlgItem(hwnd, IDC_SHOW_TYPE);
						int showType = ComboBox_GetCurSel(comboBox);
						theApp.setShowType(showType);
					}

					break;
				}
			case IDC_UPDATE_MODE:
				{
					if (code == CBN_SELCHANGE)
					{
						HWND comboBox = ::GetDlgItem(hwnd, IDC_UPDATE_MODE);
						int updateMode = ComboBox_GetCurSel(comboBox);
						theApp.setUpdateMode(updateMode);
					}

					break;
				}
			case IDC_XOR_MODE:
				{
					if (code == CBN_SELCHANGE)
					{
						HWND comboBox = ::GetDlgItem(hwnd, IDC_XOR_MODE);
						int xorMode = ComboBox_GetCurSel(comboBox);
						theApp.setXORMode(xorMode);
					}

					break;
				}
			case IDC_PEN_COLOR:
				{
					theApp.selectPenColor();

					break;
				}
			case IDC_BRUSH_COLOR:
				{
					theApp.selectBrushColor();

					break;
				}
			case IDC_CLEAR:
				{
					theApp.clearAllCache();

					break;
				}
			case IDC_SHOW_FULL:
				{
					theApp.showFull();

					break;
				}
			case IDC_SHOW_WAVEFORM:
				{
					HWND button = ::GetDlgItem(hwnd, IDC_SHOW_WAVEFORM);
					BOOL showWaveform = BST_CHECKED == Button_GetCheck(button);
					theApp.setShowWaveform(showWaveform);

					break;
				}
			case IDC_SHOW_TEXT:
				{
					HWND button = ::GetDlgItem(hwnd, IDC_SHOW_TEXT);
					BOOL showText = BST_CHECKED == Button_GetCheck(button);
					theApp.setShowText(showText);

					break;
				}
			case IDC_NO_SCROLL_TEXT:
				{
					HWND button = ::GetDlgItem(hwnd, IDC_NO_SCROLL_TEXT);
					BOOL noScrollText = BST_CHECKED == Button_GetCheck(button);
					theApp.setNoScrollText(noScrollText);

					break;
				}
			case IDC_BEHIND:
				{
					HWND button = ::GetDlgItem(hwnd, IDC_BEHIND);
					BOOL behind = BST_CHECKED == Button_GetCheck(button);
					theApp.setBehind(behind);

					break;
				}
			case IDC_INCLUDE_LAYERS:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
					{
						TCHAR text[MAX_PATH] = {};
						::GetDlgItemText(hwnd, id, text, std::size(text));
						theApp.setIncludeLayers(text);
					}

					break;
				}
			case IDC_EXCLUDE_LAYERS:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
					{
						TCHAR text[MAX_PATH] = {};
						::GetDlgItemText(hwnd, id, text, std::size(text));
						theApp.setExcludeLayers(text);
					}

					break;
				}
			case IDC_EXCLUDE_DIR:
				{
					if (code == EN_UPDATE && !m_ignoreNotification)
					{
						TCHAR text[MAX_PATH] = {};
						::GetDlgItemText(hwnd, id, text, std::size(text));
						theApp.setExcludeDir(text);
					}

					break;
				}
			}

			break;
		}
	case WM_NOTIFY:
		{
			MY_TRACE(_T("MainDialog::onDlgProc(WM_NOTIFY, 0x%08X, 0x%08X)\n"), wParam, lParam);

			switch (wParam)
			{
			case IDC_SCALE_SPIN:
				{
					NMHDR* header = (NMHDR*)lParam;
					if (header->code == UDN_DELTAPOS)
					{
						int scale = ::GetDlgItemInt(hwnd, IDC_SCALE, 0, TRUE);

						NM_UPDOWN* nm = (NM_UPDOWN*)header;
						if (nm->iDelta < 0)
						{
							scale += 10;
						}
						else
						{
							scale -= 10;
						}

						scale = std::max<int>(scale, 0);
						scale = std::min<int>(scale, 2000);

						::SetDlgItemInt(hwnd, IDC_SCALE, scale, TRUE);
				    }

					break;
				}
			}

			break;
		}
	}

	return Dialog::onDlgProc(hwnd, message, wParam, lParam);
}

//---------------------------------------------------------------------
