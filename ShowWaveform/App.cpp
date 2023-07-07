#include "pch.h"
#include "App.h"

//--------------------------------------------------------------------

App theApp;

//--------------------------------------------------------------------

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

BOOL App::func_init(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("App::func_init()\n"));

	m_fp = fp;
	m_auin.initExEditAddress();

	if (!m_subProcess.init(fp)) return FALSE;
	if (!m_subThreadManager.init(fp)) return FALSE;

	uintptr_t exedit = m_auin.GetExEdit();

	true_DrawObject = m_auin.GetDrawObject();
	true_DrawObjectText = hookAbsoluteCall(exedit + 0x0003794B, hook_DrawObjectText);

	castAddress(CallShowColorDialog, exedit + 0x0004D2A0);

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

	// サブプロセスに送信しないといけないので、カレントフレームを取得しておく。
	fp->exfunc->get_file_info(fpip->editp, &m_fi);
	m_currentFrame = fpip->frame;

	// 再生中かどうかのフラグをチェックする。
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
		if (!m_subProcess.m_windowContainer.m_inner)
			m_subProcess.m_windowContainer.delayedUpdate();
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
			createDialog();

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

			break;
		}
	case AviUtl::FilterPlugin::WindowMessage::Command:
		{
			MY_TRACE(_T("App::func_WndProc(Command, 0x%08X, 0x%08X)\n"), wParam, lParam);

			if (wParam == 0 && lParam == 0) return TRUE;

			break;
		}
	}

	return FALSE;
}

struct ProjectData {
	FullSamplesParams fullSamplesParams;
};

BOOL App::func_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size)
{
	MY_TRACE(_T("App::func_project_load()\n"));

	if (size != sizeof(ProjectData))
		return FALSE;

	auto projectData = (ProjectData*)data;

	// サブプロセスのウィンドウがまだ作成されていない場合は更新を遅らせる。
	if (!m_subProcess.m_windowContainer.m_inner)
		m_subProcess.m_windowContainer.delayedSendFullSamplesParams(&projectData->fullSamplesParams);
	else
		sendFullSamplesParams(&projectData->fullSamplesParams);

	return TRUE;
}

BOOL App::func_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size)
{
	MY_TRACE(_T("App::func_project_save()\n"));

	if (size)
		*size = sizeof(ProjectData);

	if (data)
	{
		auto projectData = (ProjectData*)data;

		FullSamplesParamsPtr params = receiveFullSamplesParams();

		if (params)
			projectData->fullSamplesParams = *params;
	}

	return TRUE;
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
	getPrivateProfileWindow(fileName, L"FullSamplesWindowContainer", m_subProcess.m_windowContainer.m_hwnd);
	getPrivateProfileWindow(fileName, L"FullSamplesDialogContainer", m_subProcess.m_dialogContainer.m_hwnd);
}

void App::save()
{
	MY_TRACE(_T("App::save()\n"));

	// ini ファイルに設定を保存する。

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
	setPrivateProfileWindow(fileName, L"FullSamplesWindowContainer", m_subProcess.m_windowContainer.m_hwnd);
	setPrivateProfileWindow(fileName, L"FullSamplesDialogContainer", m_subProcess.m_dialogContainer.m_hwnd);
}

BOOL App::createDialog()
{
	MY_TRACE(_T("App::createDialog()\n"));

	// メインダイアログを作成する。

	if (!m_mainDialog.create(m_instance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), m_fp->hwnd))
	{
		::MessageBox(m_fp->hwnd, _T("メインダイアログの作成に失敗しました"), _T("ShowWaveform"), MB_OK);

		return FALSE;
	}

	// コントロールを初期化する。

	m_mainDialog.ignoreNotification(TRUE);

	::SetDlgItemInt(m_mainDialog, IDC_SCALE, m_scale, TRUE);

	HWND showType = ::GetDlgItem(m_mainDialog, IDC_SHOW_TYPE);
	ComboBox_AddString(showType, _T("中央"));
	ComboBox_AddString(showType, _T("下"));
	ComboBox_AddString(showType, _T("上"));
	ComboBox_SetCurSel(showType, m_showType);

	HWND updateMode = ::GetDlgItem(m_mainDialog, IDC_UPDATE_MODE);
	ComboBox_AddString(updateMode, _T("更新しない"));
	ComboBox_AddString(updateMode, _T("更新する"));
	ComboBox_AddString(updateMode, _T("再生中は更新しない"));
	ComboBox_SetCurSel(updateMode, m_updateMode);

	HWND xorMode = ::GetDlgItem(m_mainDialog, IDC_XOR_MODE);
	ComboBox_AddString(xorMode, _T("通常"));
	ComboBox_AddString(xorMode, _T("XOR"));
	ComboBox_AddString(xorMode, _T("NotXOR"));
	ComboBox_AddString(xorMode, _T("Not"));
	ComboBox_SetCurSel(xorMode, m_xorMode);

	HWND showWaveform = ::GetDlgItem(m_mainDialog, IDC_SHOW_WAVEFORM);
	Button_SetCheck(showWaveform, m_showWaveform ? BST_CHECKED : BST_UNCHECKED);

	HWND showText = ::GetDlgItem(m_mainDialog, IDC_SHOW_TEXT);
	Button_SetCheck(showText, m_showText ? BST_CHECKED : BST_UNCHECKED);

	HWND noScrollText = ::GetDlgItem(m_mainDialog, IDC_NO_SCROLL_TEXT);
	Button_SetCheck(noScrollText, m_noScrollText ? BST_CHECKED : BST_UNCHECKED);

	HWND behind = ::GetDlgItem(m_mainDialog, IDC_BEHIND);
	Button_SetCheck(behind, m_behind ? BST_CHECKED : BST_UNCHECKED);

	m_mainDialog.ignoreNotification(FALSE);

	return TRUE;
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

// サブプロセスに値を送る。
BOOL App::sendFullSamplesParams(FullSamplesParams* _params)
{
	MY_TRACE(_T("App::sendFullSamplesParams()\n"));

	FullSamplesParamsPtr params = std::make_shared<FullSamplesParams>(*_params);

	return m_subThreadManager.notifyFullSamplesChanged(params);
}

// 共有メモリから値を取得する。
FullSamplesParamsPtr App::receiveFullSamplesParams()
{
	MY_TRACE(_T("App::receiveFullSamplesParams()\n"));

	return shared.getReceiverFullSamplesParams();
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

			points.emplace_back(std::max<int>(rcClip->left, rcItem->left), cy);

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

			points.emplace_back(std::max<int>(rcClip->left, rcItem->left), rcItem->bottom);

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

			points.emplace_back(std::max<int>(rcClip->left, rcItem->left), rcItem->top);

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

//--------------------------------------------------------------------

void App::setScale(int scale)
{
	m_scale = scale;

	// 拡張編集ウィンドウを再描画する。
	::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
}

void App::setShowType(int showType)
{
	m_showType = showType;

	// 拡張編集ウィンドウを再描画する。
	::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
}

void App::setUpdateMode(int updateMode)
{
	m_updateMode = updateMode;

	// 拡張編集ウィンドウを再描画する。
	::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
}

void App::setXORMode(int xorMode)
{
	m_xorMode = xorMode;

	// 拡張編集ウィンドウを再描画する。
	::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
}

void App::selectPenColor()
{
	if (IDOK == CallShowColorDialog(0, &m_penColor, 2))
	{
		// 拡張編集ウィンドウを再描画する。
		::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
	}
}

void App::selectBrushColor()
{
	if (IDOK == CallShowColorDialog(0, &m_brushColor, 2))
	{
		// 拡張編集ウィンドウを再描画する。
		::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
	}
}

void App::clearAllCache()
{
	// 全てのキャッシュをクリアする。
	m_subThreadManager.requestClear();
	m_fileCacheManager.cacheMap.clear();
	m_itemCacheManager.cacheMap.clear();

	// AviUtl を再描画する。
	::PostMessage(m_fp->hwnd, AviUtl::FilterPlugin::WindowMessage::Command, 0, 0);
}

void App::showFull()
{
	// 全体の音声波形ウィンドウを表示する。
	::ShowWindow(m_subProcess.m_windowContainer.m_hwnd, SW_SHOW);

	// WM_SHOWWINDOW が自動的には送られないので手動で送る。
	::PostMessage(m_subProcess.m_windowContainer.m_hwnd, WM_SHOWWINDOW, TRUE, 0);

	// 全体の音声波形を更新する。
	m_subThreadManager.requestRedraw();
}

void App::setShowWaveform(BOOL showWaveform)
{
	m_showWaveform = showWaveform;

	// 拡張編集ウィンドウを再描画する。
	::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
}

void App::setShowText(BOOL showText)
{
	m_showText = showText;

	// 拡張編集ウィンドウを再描画する。
	::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
}

void App::setNoScrollText(BOOL noScrollText)
{
	m_noScrollText = noScrollText;

	// 拡張編集ウィンドウを再描画する。
	::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
}

void App::setBehind(BOOL behind)
{
	m_behind = behind;

	// 拡張編集ウィンドウを再描画する。
	::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
}

//--------------------------------------------------------------------

IMPLEMENT_HOOK_PROC_NULL(void, CDECL, DrawObject, (HDC dc, int objectIndex))
{
	MY_TRACE(_T("DrawObject(%d)\n"), objectIndex);

	// 描画オブジェクトを保存しておく。
	theApp.m_currentDrawObject = theApp.m_auin.GetObject(objectIndex);
	true_DrawObject(dc, objectIndex);
	// 描画オブジェクトをクリアする。
	theApp.m_currentDrawObject = 0;
}

IMPLEMENT_HOOK_PROC_NULL(BOOL, WINAPI, DrawObjectText, (HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx))
{
	MY_TRACE(_T("DrawObjectText(0x%08X, %d, %d, 0x%08X)\n"), dc, x, y, options);
//	MY_TRACE_RECT2(rc[0]); // クリッピング矩形
//	MY_TRACE_RECT2(rc[1]); // アイテム全体の矩形

	if (theApp.m_noScrollText)
		x = std::max(70, x);

	if (!(theApp.m_currentDrawObject->flag & ExEdit::Object::Flag::Sound))
		return true_DrawObjectText(dc, x, y, options, rc, text, c, dx);

	if (!theApp.m_behind)
	{
		// フラグが立っている場合はテキストを描画する。
		if (theApp.m_showText)
			true_DrawObjectText(dc, x, y, options, rc, text, c, dx);
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
			true_DrawObjectText(dc, x, y, options, rc, text, c, dx);
	}

	return TRUE;
}

//--------------------------------------------------------------------
