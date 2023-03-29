#include "pch.h"
#include "App.h"

//---------------------------------------------------------------------

App theApp;

//---------------------------------------------------------------------

App::App()
{
}

App::~App()
{
}

BOOL App::DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		{
			MY_TRACE(_T("DLL_PROCESS_ATTACH\n"));

			// DLL インスタンスハンドルを m_instance に格納する。
			m_instance = instance;
			MY_TRACE_HEX(m_instance);

			break;
		}
	case DLL_PROCESS_DETACH:
		{
			MY_TRACE(_T("DLL_PROCESS_DETACH\n"));

			break;
		}
	}

	return TRUE;
}

void App::createControls()
{
	MY_TRACE(_T("App::createControls()\n"));

	AviUtl::SysInfo si = {};
	m_fp->exfunc->get_sys_info(0, &si);

	controls.label.scale = ::CreateWindowEx(
		0,
		WC_STATIC, _T("描画サイズ : "),
		WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE,
		0, 0, 0, 0,
		m_fp->hwnd, 0, m_instance, 0);
	::SendMessage(controls.label.scale, WM_SETFONT, (WPARAM)si.hfont, FALSE);

	controls.edit.scale = ::CreateWindowEx(
		WS_EX_CLIENTEDGE,
		WC_EDIT, 0,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		0, 0, 0, 0,
		m_fp->hwnd, (HMENU)ControlID::Edit::scale, m_instance, 0);
	::SendMessage(controls.edit.scale, WM_SETFONT, (WPARAM)si.hfont, FALSE);
	::SetDlgItemInt(m_fp->hwnd, ControlID::Edit::scale, m_scale, TRUE);

	controls.spin.scale = ::CreateWindowEx(
		0,
		UPDOWN_CLASS, 0,
		WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HORZ,
//		WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_HORZ,
		0, 0, 0, 0,
		m_fp->hwnd, (HMENU)ControlID::Spin::scale, m_instance, 0);
	::SendMessage(controls.spin.scale, WM_SETFONT, (WPARAM)si.hfont, FALSE);
//	::SendMessage(controls.spin.scale, UDM_SETBUDDY, (WPARAM)controls.edit.scale, 0);
//	::SendMessage(controls.spin.scale, UDM_SETRANGE, 0, MAKELPARAM(0, 2000));

	controls.label.showType = ::CreateWindowEx(
		0,
		WC_STATIC, _T("表示タイプ : "),
		WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE,
		0, 0, 0, 0,
		m_fp->hwnd, 0, m_instance, 0);
	::SendMessage(controls.label.showType, WM_SETFONT, (WPARAM)si.hfont, FALSE);

	controls.comboBox.showType = ::CreateWindowEx(
		0,
		WC_COMBOBOX, 0,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_HSCROLL | WS_VSCROLL |
		CBS_DROPDOWNLIST | CBS_HASSTRINGS,
		0, 0, 0, 400,
		m_fp->hwnd, (HMENU)ControlID::ComboBox::showType, m_instance, 0);
	::SendMessage(controls.comboBox.showType, WM_SETFONT, (WPARAM)si.hfont, FALSE);
	ComboBox_AddString(controls.comboBox.showType, _T("中央"));
	ComboBox_AddString(controls.comboBox.showType, _T("下"));
	ComboBox_AddString(controls.comboBox.showType, _T("上"));
	ComboBox_SetCurSel(controls.comboBox.showType, m_showType);

	controls.label.updateMode = ::CreateWindowEx(
		0,
		WC_STATIC, _T("更新モード : "),
		WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE,
		0, 0, 0, 0,
		m_fp->hwnd, 0, m_instance, 0);
	::SendMessage(controls.label.updateMode, WM_SETFONT, (WPARAM)si.hfont, FALSE);

	controls.comboBox.updateMode = ::CreateWindowEx(
		0,
		WC_COMBOBOX, 0,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_HSCROLL | WS_VSCROLL |
		CBS_DROPDOWNLIST | CBS_HASSTRINGS,
		0, 0, 0, 400,
		m_fp->hwnd, (HMENU)ControlID::ComboBox::updateMode, m_instance, 0);
	::SendMessage(controls.comboBox.updateMode, WM_SETFONT, (WPARAM)si.hfont, FALSE);
	ComboBox_AddString(controls.comboBox.updateMode, _T("更新しない"));
	ComboBox_AddString(controls.comboBox.updateMode, _T("更新する"));
	ComboBox_AddString(controls.comboBox.updateMode, _T("再生中は更新しない"));
	ComboBox_SetCurSel(controls.comboBox.updateMode, m_updateMode);

	controls.label.xorMode = ::CreateWindowEx(
		0,
		WC_STATIC, _T("描画モード : "),
		WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE,
		0, 0, 0, 0,
		m_fp->hwnd, 0, m_instance, 0);
	::SendMessage(controls.label.xorMode, WM_SETFONT, (WPARAM)si.hfont, FALSE);

	controls.comboBox.xorMode = ::CreateWindowEx(
		0,
		WC_COMBOBOX, 0,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_HSCROLL | WS_VSCROLL |
		CBS_DROPDOWNLIST | CBS_HASSTRINGS,
		0, 0, 0, 400,
		m_fp->hwnd, (HMENU)ControlID::ComboBox::xorMode, m_instance, 0);
	::SendMessage(controls.comboBox.xorMode, WM_SETFONT, (WPARAM)si.hfont, FALSE);
	ComboBox_AddString(controls.comboBox.xorMode, _T("通常"));
	ComboBox_AddString(controls.comboBox.xorMode, _T("XOR"));
	ComboBox_AddString(controls.comboBox.xorMode, _T("NotXOR"));
	ComboBox_AddString(controls.comboBox.xorMode, _T("Not"));
	ComboBox_SetCurSel(controls.comboBox.xorMode, m_xorMode);

	controls.button.penColor = ::CreateWindowEx(
		0,
		WC_BUTTON, _T("ペンの色を選択"),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		0, 0, 0, 0,
		m_fp->hwnd, (HMENU)ControlID::Button::penColor, m_instance, 0);
	::SendMessage(controls.button.penColor, WM_SETFONT, (WPARAM)si.hfont, FALSE);

	controls.button.brushColor = ::CreateWindowEx(
		0,
		WC_BUTTON, _T("ブラシの色を選択"),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		0, 0, 0, 0,
		m_fp->hwnd, (HMENU)ControlID::Button::brushColor, m_instance, 0);
	::SendMessage(controls.button.brushColor, WM_SETFONT, (WPARAM)si.hfont, FALSE);

	controls.button.clear = ::CreateWindowEx(
		0,
		WC_BUTTON, _T("キャッシュをクリア"),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		0, 0, 0, 0,
		m_fp->hwnd, (HMENU)ControlID::Button::clear, m_instance, 0);
	::SendMessage(controls.button.clear, WM_SETFONT, (WPARAM)si.hfont, FALSE);

	controls.button.showFull = ::CreateWindowEx(
		0,
		WC_BUTTON, _T("全体の音声波形を表示"),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		0, 0, 0, 0,
		m_fp->hwnd, (HMENU)ControlID::Button::showFull, m_instance, 0);
	::SendMessage(controls.button.showFull, WM_SETFONT, (WPARAM)si.hfont, FALSE);

	controls.checkBox.showWaveform = ::CreateWindowEx(
		0,
		WC_BUTTON, _T("音声波形を表示"),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
		0, 0, 0, 0,
		m_fp->hwnd, (HMENU)ControlID::CheckBox::showWaveform, m_instance, 0);
	::SendMessage(controls.checkBox.showWaveform, WM_SETFONT, (WPARAM)si.hfont, FALSE);
	if (m_showWaveform) Button_SetCheck(controls.checkBox.showWaveform, BST_CHECKED);

	controls.checkBox.showText = ::CreateWindowEx(
		0,
		WC_BUTTON, _T("テキストを表示"),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
		0, 0, 0, 0,
		m_fp->hwnd, (HMENU)ControlID::CheckBox::showText, m_instance, 0);
	::SendMessage(controls.checkBox.showText, WM_SETFONT, (WPARAM)si.hfont, FALSE);
	if (m_showText) Button_SetCheck(controls.checkBox.showText, BST_CHECKED);

	controls.checkBox.noScrollText = ::CreateWindowEx(
		0,
		WC_BUTTON, _T("テキストをスクロールしない"),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
		0, 0, 0, 0,
		m_fp->hwnd, (HMENU)ControlID::CheckBox::noScrollText, m_instance, 0);
	::SendMessage(controls.checkBox.noScrollText, WM_SETFONT, (WPARAM)si.hfont, FALSE);
	if (m_noScrollText) Button_SetCheck(controls.checkBox.noScrollText, BST_CHECKED);

	controls.checkBox.behind = ::CreateWindowEx(
		0,
		WC_BUTTON, _T("テキストより後ろに描画"),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
		0, 0, 0, 0,
		m_fp->hwnd, (HMENU)ControlID::CheckBox::behind, m_instance, 0);
	::SendMessage(controls.checkBox.behind, WM_SETFONT, (WPARAM)si.hfont, FALSE);
	if (m_behind) Button_SetCheck(controls.checkBox.behind, BST_CHECKED);
}

void App::recalcLayout()
{
	MY_TRACE(_T("App::recalcLayout()\n"));

	if (!m_fp)
		return;

	RECT rcClient; ::GetClientRect(m_fp->hwnd, &rcClient);
	RECT rcComboBox; ::GetClientRect(controls.comboBox.xorMode, &rcComboBox);
	int controlWidth = getWidth(rcClient) / 2;
	int controlHeight = getHeight(rcComboBox);

	int x1 = rcClient.left;
	int x2 = rcClient.left + controlWidth;
	int y = rcClient.top;
	int margin = 4;

	x1 += margin;
	controlWidth -= margin * 2;

	::SetWindowPos(controls.label.scale, 0, x1, y, controlWidth, controlHeight, SWP_NOZORDER);
	::SetWindowPos(controls.edit.scale, 0, x2, y, controlWidth / 2 - margin, controlHeight, SWP_NOZORDER);
	::SetWindowPos(controls.spin.scale, 0, x2 + controlWidth / 2, y, controlWidth / 2, controlHeight, SWP_NOZORDER);
	y += controlHeight + margin;
	::SetWindowPos(controls.label.showType, 0, x1, y, controlWidth, controlHeight, SWP_NOZORDER);
	::SetWindowPos(controls.comboBox.showType, 0, x2, y, controlWidth, controlHeight, SWP_NOZORDER);
	y += controlHeight + margin;
	::SetWindowPos(controls.label.updateMode, 0, x1, y, controlWidth, controlHeight, SWP_NOZORDER);
	::SetWindowPos(controls.comboBox.updateMode, 0, x2, y, controlWidth, controlHeight, SWP_NOZORDER);
	y += controlHeight + margin;
	::SetWindowPos(controls.label.xorMode, 0, x1, y, controlWidth, controlHeight, SWP_NOZORDER);
	::SetWindowPos(controls.comboBox.xorMode, 0, x2, y, controlWidth, controlHeight, SWP_NOZORDER);
	y += controlHeight + margin;
	::SetWindowPos(controls.button.penColor, 0, x1, y, controlWidth, controlHeight, SWP_NOZORDER);
	::SetWindowPos(controls.button.brushColor, 0, x2, y, controlWidth, controlHeight, SWP_NOZORDER);
	y += controlHeight + margin;
	::SetWindowPos(controls.button.clear, 0, x1, y, controlWidth, controlHeight, SWP_NOZORDER);
	::SetWindowPos(controls.button.showFull, 0, x2, y, controlWidth, controlHeight, SWP_NOZORDER);
	y += controlHeight + margin;
	::SetWindowPos(controls.checkBox.showWaveform, 0, x1, y, controlWidth, controlHeight, SWP_NOZORDER);
	::SetWindowPos(controls.checkBox.showText, 0, x2, y, controlWidth, controlHeight, SWP_NOZORDER);
	y += controlHeight + margin;
	::SetWindowPos(controls.checkBox.noScrollText, 0, x1, y, controlWidth, controlHeight, SWP_NOZORDER);
	::SetWindowPos(controls.checkBox.behind, 0, x2, y, controlWidth, controlHeight, SWP_NOZORDER);
	y += controlHeight + margin;
}

void App::load()
{
	MY_TRACE(_T("App::load()\n"));

	// ini ファイルから設定を読み込む。
	WCHAR fileName[MAX_PATH] = {};
	::GetModuleFileNameW(m_instance, fileName, MAX_PATH);
	::PathRemoveExtensionW(fileName);
	::PathAppendW(fileName, L"ShowWaveform.ini");
	MY_TRACE_WSTR(fileName);

	getPrivateProfileColor(fileName, L"Config", L"penColor", m_penColor);
	getPrivateProfileColor(fileName, L"Config", L"brushColor", m_brushColor);
	getPrivateProfileInt(fileName, L"Config", L"scale", m_scale);
	getPrivateProfileInt(fileName, L"Config", L"showType", m_showType);
	getPrivateProfileInt(fileName, L"Config", L"updateMode", m_updateMode);
	getPrivateProfileInt(fileName, L"Config", L"xorMode", m_xorMode);
	getPrivateProfileInt(fileName, L"Config", L"showWaveform", m_showWaveform);
	getPrivateProfileInt(fileName, L"Config", L"showText", m_showText);
	getPrivateProfileInt(fileName, L"Config", L"noScrollText", m_noScrollText);
	getPrivateProfileInt(fileName, L"Config", L"behind", m_behind);
	getPrivateProfileWindow(fileName, L"FullSamples", m_subProcess.m_fullSamplesContainer.m_hwnd);
}

void App::save()
{
	MY_TRACE(_T("App::save()\n"));

	// ini ファイルから設定を読み込む。
	WCHAR fileName[MAX_PATH] = {};
	::GetModuleFileNameW(m_instance, fileName, MAX_PATH);
	::PathRemoveExtensionW(fileName);
	::PathAppendW(fileName, L"ShowWaveform.ini");
	MY_TRACE_WSTR(fileName);

	setPrivateProfileColor(fileName, L"Config", L"penColor", m_penColor);
	setPrivateProfileColor(fileName, L"Config", L"brushColor", m_brushColor);
	setPrivateProfileInt(fileName, L"Config", L"scale", m_scale);
	setPrivateProfileInt(fileName, L"Config", L"showType", m_showType);
	setPrivateProfileInt(fileName, L"Config", L"updateMode", m_updateMode);
	setPrivateProfileInt(fileName, L"Config", L"xorMode", m_xorMode);
	setPrivateProfileInt(fileName, L"Config", L"showWaveform", m_showWaveform);
	setPrivateProfileInt(fileName, L"Config", L"showText", m_showText);
	setPrivateProfileInt(fileName, L"Config", L"noScrollText", m_noScrollText);
	setPrivateProfileInt(fileName, L"Config", L"behind", m_behind);
	setPrivateProfileWindow(fileName, L"FullSamples", m_subProcess.m_fullSamplesContainer.m_hwnd);
}

BOOL App::func_init(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("App::func_init()\n"));

	m_fp = fp;
	m_auin.initExEditAddress();

	if (!m_subProcess.init(fp)) return FALSE;
	if (!m_subThreadManager.init(fp)) return FALSE;

	true_DrawObject = m_auin.GetDrawObject();
	hookAbsoluteCall(m_auin.GetExEdit() + 0x0003794B, drawObjectText);

	castAddress(CallShowColorDialog, m_auin.GetExEdit() + 0x0004D2A0);

	DetourTransactionBegin();
	DetourUpdateThread(::GetCurrentThread());

	ATTACH_HOOK_PROC(DrawObject);

	if (DetourTransactionCommit() == NO_ERROR)
	{
		MY_TRACE(_T("API フックに成功しました\n"));
	}
	else
	{
		MY_TRACE(_T("API フックに失敗しました\n"));
	}

	return TRUE;
}

BOOL App::func_exit(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("App::func_exit()\n"));

	m_subThreadManager.exit(fp);
	m_subProcess.exit(fp);

	return TRUE;
}

BOOL App::func_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
{
	MY_TRACE(_T("App::func_proc()\n"));

	if (!fp->exfunc->is_editing(fpip->editp))
		return FALSE; // 編集中ではないときは何もしない。

	if (fp->exfunc->is_saving(fpip->editp))
		return FALSE; // 音声を保存するときは何もしない。

	fp->exfunc->get_file_info(fpip->editp, &m_fi);
	m_currentFrame = fpip->frame;

	BOOL isPlaying = !!((DWORD)fpip->editp->aviutl_window_info.flag & 0x00040000);

	switch (m_updateMode)
	{
	case UpdateMode::Off:
		{
			return FALSE; // 無効化されているときは何もしない。
		}
	case UpdateMode::OnWithoutPlaying:
		{
			if (isPlaying)
				return FALSE; // 再生中のときは何もしない。

			break;
		}
	}

	// サブプロセスのプロジェクトパラメータを更新する。
	updateProjectParams();

	// 再生中以外の場合は
	if (!isPlaying)
	{
		// サブプロセスのウィンドウがまだ作成されていない場合は更新を遅らせる。
		if (!m_subProcess.m_fullSamplesWindow)
			m_subProcess.m_fullSamplesContainer.delayedUpdate();
		else
			updateItemCache(TRUE);
	}

	return FALSE;
}

BOOL App::func_update(AviUtl::FilterPlugin* fp, AviUtl::FilterPlugin::UpdateStatus status)
{
	int index = fp->get_updated_idx(status);

	return TRUE;
}

BOOL App::func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
{
	switch (message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			::SendMessage(::GetWindow(hwnd, GW_OWNER), message, wParam, lParam);
			break;
		}
	case AviUtl::FilterPlugin::WindowMessage::Init:
		{
			MY_TRACE(_T("App::func_WndProc(Init, 0x%08X, 0x%08X)\n"), wParam, lParam);

			load();
			createControls();
			recalcLayout();

			m_exists = TRUE;

			break;
		}
	case AviUtl::FilterPlugin::WindowMessage::Exit:
		{
			MY_TRACE(_T("App::func_WndProc(Exit, 0x%08X, 0x%08X)\n"), wParam, lParam);

			m_exists = FALSE;

			save();

			break;
		}
	case WM_SIZE:
		{
			MY_TRACE(_T("App::func_WndProc(WM_SIZE, 0x%08X, 0x%08X)\n"), wParam, lParam);

			recalcLayout();

			break;
		}
	case AviUtl::FilterPlugin::WindowMessage::Command:
		{
			MY_TRACE(_T("App::func_WndProc(Command, 0x%08X, 0x%08X)\n"), wParam, lParam);

			if (wParam == 0 && lParam == 0) return TRUE;

			break;
		}
	case WM_COMMAND:
		{
			UINT code = HIWORD(wParam);
			UINT id = LOWORD(wParam);
			HWND sender = (HWND)lParam;

			switch (id)
			{
			case ControlID::Edit::scale:
				{
					if (code == EN_UPDATE)
					{
						m_scale = (int)::GetDlgItemInt(hwnd, ControlID::Edit::scale, 0, TRUE);

						// 拡張編集ウィンドウを再描画する。
						::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
					}

					break;
				}
			case ControlID::ComboBox::showType:
				{
					m_showType = ComboBox_GetCurSel(controls.comboBox.showType);

					// 拡張編集ウィンドウを再描画する。
					::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);

					break;
				}
			case ControlID::ComboBox::updateMode:
				{
					m_updateMode = ComboBox_GetCurSel(controls.comboBox.updateMode);

					// 拡張編集ウィンドウを再描画する。
					::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);

					break;
				}
			case ControlID::ComboBox::xorMode:
				{
					m_xorMode = ComboBox_GetCurSel(controls.comboBox.xorMode);

					// 拡張編集ウィンドウを再描画する。
					::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);

					break;
				}
			case ControlID::Button::penColor:
				{
					if (IDOK == CallShowColorDialog(0, &m_penColor, 2))
					{
						// 拡張編集ウィンドウを再描画する。
						::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
					}

					break;
				}
			case ControlID::Button::brushColor:
				{
					if (IDOK == CallShowColorDialog(0, &m_brushColor, 2))
					{
						// 拡張編集ウィンドウを再描画する。
						::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
					}

					break;
				}
			case ControlID::Button::clear:
				{
					// 全てのキャッシュをクリアする。
					m_subThreadManager.requestClear();
					m_fileCacheManager.cacheMap.clear();
					m_itemCacheManager.cacheMap.clear();

					// AviUtl を再描画する。
					::PostMessage(hwnd, AviUtl::FilterPlugin::WindowMessage::Command, 0, 0);

					break;
				}
			case ControlID::Button::showFull:
				{
					// 全体の音声波形ウィンドウを表示する。
					::ShowWindow(m_subProcess.m_fullSamplesContainer.m_hwnd, SW_SHOW);

					// WM_SHOWWINDOW が自動的には送られないので手動で送る。
					::PostMessage(m_subProcess.m_fullSamplesContainer.m_hwnd, WM_SHOWWINDOW, TRUE, 0);

					// 全体の音声波形を更新する。
					m_subThreadManager.requestRedraw();

					break;
				}
			case ControlID::CheckBox::showWaveform:
				{
					m_showWaveform = BST_CHECKED == Button_GetCheck(controls.checkBox.showWaveform);

					// 拡張編集ウィンドウを再描画する。
					::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);

					break;
				}
			case ControlID::CheckBox::showText:
				{
					m_showText = BST_CHECKED == Button_GetCheck(controls.checkBox.showText);

					// 拡張編集ウィンドウを再描画する。
					::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);

					break;
				}
			case ControlID::CheckBox::noScrollText:
				{
					m_noScrollText = BST_CHECKED == Button_GetCheck(controls.checkBox.noScrollText);

					// 拡張編集ウィンドウを再描画する。
					::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);

					break;
				}
			case ControlID::CheckBox::behind:
				{
					m_behind = BST_CHECKED == Button_GetCheck(controls.checkBox.behind);

					// 拡張編集ウィンドウを再描画する。
					::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);

					break;
				}
			}

			break;
		}
	case WM_NOTIFY:
		{
			MY_TRACE(_T("App::func_WndProc(WM_NOTIFY, 0x%08X, 0x%08X)\n"), wParam, lParam);

			switch (wParam)
			{
			case ControlID::Spin::scale:
				{
					NMHDR* header = (NMHDR*)lParam;
					if (header->code == UDN_DELTAPOS)
					{
						int scale = ::GetDlgItemInt(hwnd, ControlID::Edit::scale, 0, TRUE);

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

						::SetDlgItemInt(hwnd, ControlID::Edit::scale, scale, TRUE);
				    }

					break;
				}
			}

			break;
		}
	}

	return FALSE;
}

inline BOOL checkMin(std::vector<POINT>& points, int x, int y)
{
	POINT& back = points.back();
	if (back.x != x) return TRUE;
	back.y = std::min<int>(back.y, y);
	return FALSE;
}

inline BOOL checkMax(std::vector<POINT>& points, int x, int y)
{
	POINT& back = points.back();
	if (back.x != x) return TRUE;
	back.y = std::max<int>(back.y, y);
	return FALSE;
}

BOOL App::updateProjectParams()
{
	MY_TRACE(_T("App::updateProjectParams()\n"));

	ProjectParamsPtr params = std::make_shared<ProjectParams>();
	params->video_scale = m_fi.video_scale;
	params->video_rate = m_fi.video_rate;
	params->frameNumber = m_fi.frame_n;
	params->sceneIndex = m_auin.GetCurrentSceneIndex();
	params->currentFrame = m_currentFrame;
	return m_subThreadManager.notifyProjectChanged(params);
}

BOOL App::updateItemCache(BOOL send)
{
	MY_TRACE(_T("App::updateItemCache(%d)\n"), send);

	return m_itemCacheManager.update(send);
}

void App::drawWaveform(HDC dc, LPCRECT rcClip, LPCRECT rcItem)
{
	MY_TRACE(_T("App::drawWaveform()\n"));

	if (!m_currentDrawObject) return;
	if (!m_fi.video_scale) return;
	if (!m_fi.video_rate) return;

	ItemCachePtr cache = m_itemCacheManager.getCache(m_currentDrawObject);
	if (!cache) return;

	int scale = std::max(1, m_scale);

	int w = rcItem->right - rcItem->left;
	int h = rcItem->bottom - rcItem->top;
	int cy = (rcItem->top + rcItem->bottom) / 2;

	int c = cache->samples.size();
	MY_TRACE_INT(c);

	std::vector<POINT> points;

	switch (m_showType)
	{
	case ShowType::Both:
		{
			// min と max を描画する。

			points.emplace_back(std::max(rcClip->left, rcItem->left), cy);

			int i = 0;

			for (; i < c; i++)
			{
				int x = (int)m_auin.FrameToX(i + m_currentDrawObject->frame_begin);

				if (x > rcClip->left) break;
			}

			int checkIndex = i = std::max<int>(0, i - 1);

			for (; i < c; i++)
			{
				const Sample& sample = cache->samples[i];
				int x = (int)m_auin.FrameToX(i + m_currentDrawObject->frame_begin);
				int y = cy - (int)(sample.level * scale);

				if (i != checkIndex && checkMin(points, x, y))
					points.emplace_back(x, y);

				if (x > rcClip->right) break;
			}

			int lx = (int)m_auin.FrameToX(i + m_currentDrawObject->frame_begin);
			lx = std::min<int>(lx, rcClip->right);

			points.emplace_back(lx, points.back().y);
			points.emplace_back(lx, cy);
			points.emplace_back(lx, points.back().y);

			checkIndex = i = std::min<int>(i, c - 1);

			for (; i >= 0; i--)
			{
				const Sample& sample = cache->samples[i];
				int x = (int)m_auin.FrameToX(i + m_currentDrawObject->frame_begin);
				int y = cy + (int)(sample.level * scale);

				if (i != checkIndex && checkMax(points, x, y))
					points.emplace_back(x, y);

				if (x < rcClip->left) break;
			}

			break;
		}
	case ShowType::Min:
		{
			// min のみを描画する。

			points.emplace_back(std::max(rcClip->left, rcItem->left), rcItem->bottom);

			int i = 0;

			for (; i < c; i++)
			{
				int x = (int)m_auin.FrameToX(i + m_currentDrawObject->frame_begin);

				if (x > rcClip->left) break;
			}

			int checkIndex = i = std::max<int>(0, i - 1);

			for (; i < c; i++)
			{
				const Sample& sample = cache->samples[i];
				int x = (int)m_auin.FrameToX(i + m_currentDrawObject->frame_begin);
				int y = rcItem->bottom - (int)(sample.level * scale);

				if (i != checkIndex && checkMin(points, x, y))
					points.emplace_back(x, y);

				if (x > rcClip->right) break;
			}

			int lx = (int)m_auin.FrameToX(i + m_currentDrawObject->frame_begin);
			lx = std::min<int>(lx, rcClip->right);

			points.emplace_back(lx, points.back().y);
			points.emplace_back(lx, rcItem->bottom);

			break;
		}
	case ShowType::Max:
		{
			// max のみを描画する。

			points.emplace_back(std::max(rcClip->left, rcItem->left), rcItem->top);

			int i = 0;

			for (; i < c; i++)
			{
				int x = (int)m_auin.FrameToX(i + m_currentDrawObject->frame_begin);

				if (x > rcClip->left) break;
			}

			int checkIndex = i = std::max<int>(0, i - 1);

			for (; i < c; i++)
			{
				const Sample& sample = cache->samples[i];
				int x = (int)m_auin.FrameToX(i + m_currentDrawObject->frame_begin);
				int y = rcItem->top + (int)(sample.level * scale);

				if (i != checkIndex && checkMax(points, x, y))
					points.emplace_back(x, y);

				if (x > rcClip->right) break;
			}

			int lx = (int)m_auin.FrameToX(i + m_currentDrawObject->frame_begin);
			lx = std::min<int>(lx, rcClip->right);

			points.emplace_back(lx, points.back().y);
			points.emplace_back(lx, rcItem->top);

			break;
		}
	}

	HPEN pen = (m_penColor != CLR_NONE) ? ::CreatePen(PS_SOLID, 1, m_penColor) : (HPEN)::GetStockObject(NULL_PEN);
	HPEN oldPen = (HPEN)::SelectObject(dc, pen);
	HBRUSH brush = (m_brushColor != CLR_NONE) ? ::CreateSolidBrush(m_brushColor) : (HBRUSH)::GetStockObject(NULL_BRUSH);
	HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, brush);
	int rop2 = ::GetROP2(dc);
	switch (m_xorMode) {
	case XORMode::XOR: ::SetROP2(dc, R2_XORPEN); break;
	case XORMode::NotXOR: ::SetROP2(dc, R2_NOTXORPEN); break;
	case XORMode::Not: ::SetROP2(dc, R2_NOT); break;
	}
	::Polygon(dc, points.data(), points.size());
	::SetROP2(dc, rop2);
	::SelectObject(dc, oldBrush);
	if (m_brushColor != CLR_NONE) ::DeleteObject(brush);
	::SelectObject(dc, oldPen);
	if (m_penColor != CLR_NONE) ::DeleteObject(pen);
}

//---------------------------------------------------------------------

IMPLEMENT_HOOK_PROC_NULL(void, CDECL, DrawObject, (HDC dc, int objectIndex))
{
//	MY_TRACE(_T("DrawObject(%d)\n"), objectIndex);

	// 描画オブジェクトを保存しておく。
	theApp.m_currentDrawObject = theApp.m_auin.GetObject(objectIndex);
	true_DrawObject(dc, objectIndex);
	// 描画オブジェクトをクリアする。
	theApp.m_currentDrawObject = 0;
}

BOOL WINAPI drawObjectText(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
{
//	MY_TRACE(_T("drawObjectText(0x%08X, %d, %d, 0x%08X)\n"), dc, x, y, options);
//	MY_TRACE_RECT2(rc[0]); // クリッピング矩形
//	MY_TRACE_RECT2(rc[1]); // アイテム全体の矩形

	if (theApp.m_noScrollText)
		x = std::max(70, x);

	if (!(theApp.m_currentDrawObject->flag & ExEdit::Object::Flag::Sound))
		return ::ExtTextOut(dc, x, y, options, rc, text, c, dx);

	if (!theApp.m_behind)
	{
		// フラグが立っている場合はテキストを描画する。
		if (theApp.m_showText)
			::ExtTextOut(dc, x, y, options, rc, text, c, dx);
	}

	// フラグが立っている場合は音声波形を描画する。
	if (theApp.m_showWaveform)
	{
		// 描画がはみ出てもいいようにクリッピング領域を設定しておく。
		HRGN rgn = ::CreateRectRgnIndirect(rc);
		::SelectClipRgn(dc, rgn);
		::DeleteObject(rgn);

		// 音声波形を描画する。
		theApp.drawWaveform(dc, &rc[0], &rc[1]);

		// クリッピングを解除する。
		::SelectClipRgn(dc, 0);
	}

	if (theApp.m_behind)
	{
		// フラグが立っている場合はテキストを描画する。
		if (theApp.m_showText)
			::ExtTextOut(dc, x, y, options, rc, text, c, dx);
	}

	return TRUE;
}

//---------------------------------------------------------------------
