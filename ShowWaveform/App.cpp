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

void App::load(int* track_def, int* check_def)
{
	// ini ファイルから設定を読み込む。
	WCHAR fileName[MAX_PATH] = {};
	::GetModuleFileNameW(m_instance, fileName, MAX_PATH);
	::PathRenameExtensionW(fileName, L".ini");
	MY_TRACE_WSTR(fileName);

	getPrivateProfileColor(fileName, L"Config", L"penColor", m_penColor);
	getPrivateProfileColor(fileName, L"Config", L"brushColor", m_brushColor);
	getPrivateProfileInt(fileName, L"Config", L"scale", track_def[Track::Scale]);
	getPrivateProfileInt(fileName, L"Config", L"showType", track_def[Track::ShowType]);
	getPrivateProfileInt(fileName, L"Config", L"updateMode", track_def[Track::UpdateMode]);
	getPrivateProfileInt(fileName, L"Config", L"xorMode", track_def[Track::XORMode]);
	getPrivateProfileInt(fileName, L"Config", L"showWaveform", check_def[Check::ShowWaveform]);
	getPrivateProfileInt(fileName, L"Config", L"showText", check_def[Check::ShowText]);
	getPrivateProfileInt(fileName, L"Config", L"noScrollText", check_def[Check::NoScrollText]);
}

void App::save(int* track_def, int* check_def)
{
	// ini ファイルから設定を読み込む。
	WCHAR fileName[MAX_PATH] = {};
	::GetModuleFileNameW(m_instance, fileName, MAX_PATH);
	::PathRenameExtensionW(fileName, L".ini");
	MY_TRACE_WSTR(fileName);

	setPrivateProfileColor(fileName, L"Config", L"penColor", m_penColor);
	setPrivateProfileColor(fileName, L"Config", L"brushColor", m_brushColor);
	setPrivateProfileInt(fileName, L"Config", L"scale", track_def[Track::Scale]);
	setPrivateProfileInt(fileName, L"Config", L"showType", track_def[Track::ShowType]);
	setPrivateProfileInt(fileName, L"Config", L"updateMode", track_def[Track::UpdateMode]);
	setPrivateProfileInt(fileName, L"Config", L"xorMode", track_def[Track::XORMode]);
	setPrivateProfileInt(fileName, L"Config", L"showWaveform", check_def[Check::ShowWaveform]);
	setPrivateProfileInt(fileName, L"Config", L"showText", check_def[Check::ShowText]);
	setPrivateProfileInt(fileName, L"Config", L"noScrollText", check_def[Check::NoScrollText]);
}

BOOL App::func_init(AviUtl::FilterPlugin* fp)
{
	MY_TRACE(_T("App::func_init()\n"));

	m_fp = fp;
	m_auin.initExEditAddress();

	if (!m_subProcess.init(fp))
		return FALSE;

	if (!m_subThreadManager.init(fp))
		return FALSE;

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

	int updateMode = fp->track[Track::UpdateMode];

	if (updateMode == 0)
		return FALSE; // 更新モードが 0 のときは何もしない。

	m_itemCacheManager.update(TRUE);

	return FALSE;
}

BOOL App::func_update(AviUtl::FilterPlugin* fp, AviUtl::FilterPlugin::UpdateStatus status)
{
	int index = fp->get_updated_idx(status);

	if (index == Track::Scale ||
		index == Track::ShowType ||
		index == Check::ShowWaveform ||
		index == Check::ShowText ||
		index == Check::NoScrollText)
	{
		// 拡張編集ウィンドウを再描画する。
		::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
	}

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
	case AviUtl::FilterPlugin::WindowMessage::Command:
		{
			MY_TRACE(_T("App::func_WndProc(Command, 0x%08X, 0x%08X)\n"), wParam, lParam);

			if (wParam == 0 && lParam == 0) return TRUE;

			int index = LOWORD(wParam);
			MY_TRACE_INT(index);

			break;
		}
	case WM_COMMAND:
		{
			int index = LOWORD(wParam) - AviUtl::FilterPlugin::MidFilterButton;
			MY_TRACE_INT(index);

			switch (index)
			{
			case Check::Clear:
				{
					::PostMessage(m_subProcess.m_mainWindow, WM_AVIUTL_FILTER_CLEAR, 0, 0);

					m_fileCacheManager.cacheMap.clear();
					m_itemCacheManager.cacheMap.clear();

					// AviUtl を再描画する。
					::PostMessage(hwnd, AviUtl::FilterPlugin::WindowMessage::Command, 0, 0);

					break;
				}
			case Check::PenColor:
				{
					if (IDOK == CallShowColorDialog(0, &m_penColor, 2))
					{
						// 拡張編集ウィンドウを再描画する。
						::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
					}

					break;
				}
			case Check::BrushColor:
				{
					if (IDOK == CallShowColorDialog(0, &m_brushColor, 2))
					{
						// 拡張編集ウィンドウを再描画する。
						::InvalidateRect(m_auin.GetExEditWindow(), 0, FALSE);
					}

					break;
				}
			}

			break;
		}
	}

	if (message == WM_AVIUTL_FILTER_INIT)
	{
		MY_TRACE(_T("func_WndProc(WM_AVIUTL_FILTER_INIT, 0x%08X, 0x%08X)\n"), wParam, lParam);

		m_subProcess.m_mainWindow = (HWND)wParam;

		int updateMode = fp->track[Track::UpdateMode];

		if (updateMode == 0)
			return FALSE; // 更新モードが 0 のときは何もしない。

		// AviUtl を再描画したときに func_proc() で呼ばれるので必要ない。
//		if (m_itemCacheManager.update(TRUE))
		{
			// AviUtl を再描画する。
			::PostMessage(hwnd, AviUtl::FilterPlugin::WindowMessage::Command, 0, 0);
		}
	}
	else if (message == WM_AVIUTL_FILTER_RECEIVE)
	{
		MY_TRACE(_T("func_WndProc(WM_AVIUTL_FILTER_RECEIVE, 0x%08X, 0x%08X)\n"), wParam, lParam);

		m_fileCacheManager.receiveCache();

		if (m_itemCacheManager.update(FALSE))
		{
			// ↑で拡張編集ウィンドウの再描画が行われるので必要ない。
//			::PostMessage(hwnd, AviUtl::FilterPlugin::WindowMessage::Command, 0, 0);
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

void App::drawWaveform(HDC dc, LPCRECT rcClip, LPCRECT rcItem)
{
	if (!m_currentDrawObject) return;
	if (!m_fi.video_scale) return;
	if (!m_fi.video_rate) return;

	ItemCachePtr cache = m_itemCacheManager.getCache(m_currentDrawObject);
	if (!cache) return;

	int scale = std::max(1, m_fp->track[Track::Scale]);

	int w = rcItem->right - rcItem->left;
	int h = rcItem->bottom - rcItem->top;
	int cy = (rcItem->top + rcItem->bottom) / 2;

	int c = cache->samples.size();
	MY_TRACE_INT(c);

	std::vector<POINT> points;

	switch (m_fp->track[Track::ShowType])
	{
	case 0:
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
	case 1:
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
	case 2:
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
	switch (m_fp->track[Track::XORMode]) {
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
	MY_TRACE(_T("DrawObject(%d)\n"), objectIndex);

	// 描画オブジェクトを保存しておく。
	theApp.m_currentDrawObject = theApp.m_auin.GetObject(objectIndex);
	true_DrawObject(dc, objectIndex);
	// 描画オブジェクトをクリアする。
	theApp.m_currentDrawObject = 0;
}

BOOL WINAPI drawObjectText(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
{
	MY_TRACE(_T("drawObjectText(0x%08X, %d, %d, 0x%08X)\n"), dc, x, y, options);
	MY_TRACE_RECT2(rc[0]); // クリッピング矩形
	MY_TRACE_RECT2(rc[1]); // アイテム全体の矩形

	if (theApp.m_fp->check[Check::NoScrollText])
		x = std::max(70, x);

	if (!(theApp.m_currentDrawObject->flag & ExEdit::Object::Flag::Sound))
		return ::ExtTextOut(dc, x, y, options, rc, text, c, dx);

	// フラグが立っている場合はテキストを描画する。
	if (theApp.m_fp->check[Check::ShowText])
		::ExtTextOut(dc, x, y, options, rc, text, c, dx);

	// フラグが立っている場合は音声波形を描画する。
	if (theApp.m_fp->check[Check::ShowWaveform])
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

	return TRUE;
}

//---------------------------------------------------------------------
