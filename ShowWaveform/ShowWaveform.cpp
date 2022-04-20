#include "pch.h"
#include "ShowWaveform.h"

//---------------------------------------------------------------------

CShowWaveformApp theApp;

// デバッグ用コールバック関数。デバッグメッセージを出力する。
void ___outputLog(LPCTSTR text, LPCTSTR output)
{
	::OutputDebugString(output);
}

CShowWaveformApp::CShowWaveformApp()
{
	m_instance = 0;
	m_fp = 0;
	m_currentDrawObject = 0;

	m_FrameToX = 0;

	m_penColor = RGB(0x00, 0xff, 0xff);
	m_brushColor = RGB(0x00, 0xff, 0xff);
}

CShowWaveformApp::~CShowWaveformApp()
{
}

BOOL CShowWaveformApp::DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
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

void CShowWaveformApp::load(int* track_def)
{
	// ini ファイルから設定を読み込む。
	WCHAR fileName[MAX_PATH] = {};
	::GetModuleFileNameW(m_instance, fileName, MAX_PATH);
	::PathRenameExtensionW(fileName, L".ini");
	MY_TRACE_WSTR(fileName);

	getPrivateProfileColor(fileName, L"Config", L"penColor", m_penColor);
	getPrivateProfileColor(fileName, L"Config", L"brushColor", m_brushColor);
	getPrivateProfileInt(fileName, L"Config", L"frameInc", track_def[0]);
	getPrivateProfileInt(fileName, L"Config", L"sampleInc", track_def[1]);
	getPrivateProfileInt(fileName, L"Config", L"scaleDiv", track_def[2]);
	getPrivateProfileInt(fileName, L"Config", L"showType", track_def[3]);
}

BOOL CShowWaveformApp::func_init(FILTER *fp)
{
	MY_TRACE(_T("CShowWaveformApp::func_init()\n"));

	m_fp = fp;

	// 拡張編集の関数や変数を取得する。
	DWORD exedit = (DWORD)::GetModuleHandle(_T("exedit.auf"));
	g_exeditWindow = (HWND*)(exedit + 0x177A44);
	g_settingDialog = (HWND*)(exedit + 0x1539C8);
	g_objectTable = (auls::EXEDIT_OBJECT**)(exedit + 0x168FA8);
	g_filterTable = (auls::EXEDIT_FILTER**)(exedit + 0x187C98);
	g_objectIndex = (int*)(exedit + 0x177A10);
	g_filterIndex = (int*)(exedit + 0x14965C);
	g_objectCount = (int*)(exedit + 0x146250);
	g_currentSceneObjectCount = (int*)(exedit + 0x15918C);
	g_objectData = (auls::EXEDIT_OBJECT**)(exedit + 0x1E0FA4);
	g_objectExdata = (BYTE**)(exedit + 0x1E0FA8);
	g_nextObject = (int*)(exedit + 0x1592d8);
	g_editp = (void**)(exedit + 0x1a532c);
	true_DrawObject = (Type_DrawObject)(exedit + 0x00037060);
	m_FrameToX = (Type_FrameToX)(exedit + 0x00032BD0);

	hookAbsoluteCall(exedit + 0x0003794B, drawObjectText);

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

	fp->exfunc->add_menu_item(fp, (LPSTR)"選択アイテムを更新", fp->hwnd, CHECK_UPDATE_SELECTED_ITEM, 0, 0);
	fp->exfunc->add_menu_item(fp, (LPSTR)"すべてのアイテムを更新", fp->hwnd, CHECK_UPDATE_ALL_ITEMS, 0, 0);
	fp->exfunc->add_menu_item(fp, (LPSTR)"選択アイテムの波形を削除", fp->hwnd, CHECK_DELETE_SELECTED_ITEM, 0, 0);
	fp->exfunc->add_menu_item(fp, (LPSTR)"すべてのアイテムの波形を削除", fp->hwnd, CHECK_DELETE_ALL_ITEMS, 0, 0);

	return TRUE;
}

BOOL CShowWaveformApp::func_exit(FILTER *fp)
{
	MY_TRACE(_T("CShowWaveformApp::func_exit()\n"));

	return TRUE;
}

BOOL CShowWaveformApp::func_proc(FILTER *fp, FILTER_PROC_INFO *fpip)
{
	MY_TRACE(_T("CShowWaveformApp::func_proc()\n"));

	if (!fp->check[CHECK_UPDATE_ALWAYS])
		return FALSE;

	for (auto it = m_waveformMap.begin(); it != m_waveformMap.end();)
	{
		auls::EXEDIT_OBJECT* object = it->first;

		if (!object->exists || !(object->type & auls::EXEDIT_OBJECT::TYPE_SOUND))
		{
			m_waveformMap.erase(it++);
		}
		else
		{
			it++;
		}
	}

	GetWaveform gw(fp);

	int c = m_waveformMap.size();

	for (auto it = m_waveformMap.begin(); it != m_waveformMap.end(); it++)
	{
		auls::EXEDIT_OBJECT* object = it->first;
		WaveformPtr waveform = it->second;

		if (memcmp(object, &waveform->m_objectCopy, sizeof(waveform->m_objectCopy)) == 0)
			continue;

		gw.get(object);
	}

	return FALSE;
}

BOOL CShowWaveformApp::func_update(FILTER *fp, int status)
{
	if (status == FILTER_UPDATE_STATUS_TRACK + TRACK_SCALE_DIV ||
		status == FILTER_UPDATE_STATUS_TRACK + TRACK_SHOW_TYPE ||
		status == FILTER_UPDATE_STATUS_CHECK + CHECK_UPDATE_SELECTED_ITEM ||
		status == FILTER_UPDATE_STATUS_CHECK + CHECK_UPDATE_ALL_ITEMS ||
		status == FILTER_UPDATE_STATUS_CHECK + CHECK_DELETE_SELECTED_ITEM ||
		status == FILTER_UPDATE_STATUS_CHECK + CHECK_DELETE_ALL_ITEMS ||
		status == FILTER_UPDATE_STATUS_CHECK + CHECK_SHOW_WAVEFORM ||
		status == FILTER_UPDATE_STATUS_CHECK + CHECK_SHOW_TEXT)
	{
		::InvalidateRect(Exedit_GetExeditWindow(), 0, FALSE);
	}

	return TRUE;
}

BOOL CShowWaveformApp::func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, void *editp, FILTER *fp)
{
	switch (message)
	{
	case WM_FILTER_CHANGE_WINDOW:
		{
			MY_TRACE(_T("CShowWaveformApp::func_WndProc(WM_FILTER_CHANGE_WINDOW)\n"));

			break;
		}
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			::SendMessage(::GetWindow(hwnd, GW_OWNER), message, wParam, lParam);
			break;
		}
	case WM_FILTER_COMMAND:
		{
			int index = LOWORD(wParam);
			MY_TRACE_INT(index);

			break;
		}
	case WM_COMMAND:
		{
			int index = LOWORD(wParam) - MID_FILTER_BUTTON;
			MY_TRACE_INT(index);

			if (index == CHECK_UPDATE_SELECTED_ITEM) getWaveform(fp);
			else if (index == CHECK_UPDATE_ALL_ITEMS) getAllWaveform(fp);
			else if (index == CHECK_DELETE_SELECTED_ITEM) deleteWaveform(fp);
			else if (index == CHECK_DELETE_ALL_ITEMS) deleteAllWaveform(fp);

			break;
		}
	}

	return FALSE;
}

// 選択アイテムの音声波形を取得する。
void CShowWaveformApp::getWaveform(FILTER *fp)
{
	MY_TRACE(_T("CShowWaveformApp::getWaveform()\n"));

	// 音声波形の取得に必要なデータを取得する。
	GetWaveform gw(fp);

	// 選択アイテムを取得する。
	int objectIndex = Exedit_GetCurrentObjectIndex();
	MY_TRACE_INT(objectIndex);
	if (objectIndex < 0) return;

	// 中間点元を取得する。
	int midptLeader = Exedit_GetObject(objectIndex)->index_midpt_leader;
	MY_TRACE_INT(midptLeader);
	if (midptLeader >= 0)
		objectIndex = midptLeader; // 中間点がある場合は中間点元のオブジェクト ID を取得

	while (objectIndex >= 0)
	{
		// オブジェクトを取得する。
		auls::EXEDIT_OBJECT* object = Exedit_GetObject(objectIndex);
		MY_TRACE_HEX(object);
		if (!object) break;

		// 中間点元を取得する。
		int midptLeader2 = object->index_midpt_leader;
		MY_TRACE_INT(midptLeader2);
		if (midptLeader2 != midptLeader)
			break; // 中間点元が違う場合はループ終了。

		// アイテムの音声波形を取得する。
		gw.get(object);

		if (midptLeader < 0)
			break; // 中間点が存在しない場合はここでループ終了。

		// 次の区間のオブジェクトを取得する。
		objectIndex = Exedit_GetNextObjectIndex(objectIndex);
	}
}

// すべてのアイテムの音声波形を取得する。
void CShowWaveformApp::getAllWaveform(FILTER *fp)
{
	MY_TRACE(_T("CShowWaveformApp::getAllWaveform()\n"));

	// 音声波形の取得に必要なデータを取得する。
	GetWaveform gw(fp);

	// 現在保持している音声波形を消去する。
//	m_waveformMap.clear();

	// 現在のシーンのアイテムを列挙する。
	int c = Exedit_GetCurrentSceneObjectCount();
	for (int i = 0; i < c; i++)
	{
		auls::EXEDIT_OBJECT* object = Exedit_GetSortedObject(i);
		if (!(object->type & auls::EXEDIT_OBJECT::TYPE_SOUND))
			continue; // 音声波形を取得できるのはサウンドタイプのアイテムのみ。

		// アイテムの音声波形を取得する。
		gw.get(object);
	}
}

// 選択アイテムの音声波形を消去する。
void CShowWaveformApp::deleteWaveform(FILTER *fp)
{
	MY_TRACE(_T("CShowWaveformApp::deleteWaveform()\n"));

	// 選択アイテムを取得する。
	int objectIndex = Exedit_GetCurrentObjectIndex();
	MY_TRACE_INT(objectIndex);
	if (objectIndex < 0) return;

	// 中間点元を取得する。
	int midptLeader = Exedit_GetObject(objectIndex)->index_midpt_leader;
	MY_TRACE_INT(midptLeader);
	if (midptLeader >= 0)
		objectIndex = midptLeader; // 中間点がある場合は中間点元のオブジェクト ID を取得

	while (objectIndex >= 0)
	{
		// オブジェクトを取得する。
		auls::EXEDIT_OBJECT* object = Exedit_GetObject(objectIndex);
		MY_TRACE_HEX(object);
		if (!object) break;

		// 中間点元を取得する。
		int midptLeader2 = object->index_midpt_leader;
		MY_TRACE_INT(midptLeader2);
		if (midptLeader2 != midptLeader)
			break; // 中間点元が違う場合はループ終了。

		// 音声波形を消去する。
		m_waveformMap.erase(object);

		if (midptLeader < 0)
			break; // 中間点が存在しない場合はここでループ終了。

		// 次の区間のオブジェクトを取得する。
		objectIndex = Exedit_GetNextObjectIndex(objectIndex);
	}

	// 拡張編集ウィンドウを再描画する。
	::InvalidateRect(Exedit_GetExeditWindow(), 0, FALSE);
}

// すべてのアイテムの音声波形を消去する。
void CShowWaveformApp::deleteAllWaveform(FILTER *fp)
{
	MY_TRACE(_T("CShowWaveformApp::deleteAllWaveform()\n"));

	// すべての音声波形を消去する。
	m_waveformMap.clear();

	// 拡張編集ウィンドウを再描画する。
	::InvalidateRect(Exedit_GetExeditWindow(), 0, FALSE);
}

// 音声波形の取得に必要なデータを取得する。
CShowWaveformApp::GetWaveform::GetWaveform(FILTER *fp)
{
	// 開始時間を取得する。
	startTime = ::timeGetTime();

	// AviUtl フィルタオブジェクトを取得する。
	filter = auls::AviUtl_GetFilter(fp, "拡張編集(音声)");
	// エディットハンドルを取得する。
	editp = theApp.Exedit_GetEditp();

	// ファイル情報を取得する。
	fp->exfunc->get_file_info(editp, &fi);
	fi.audio_n = fi.audio_rate / fi.video_rate;

	// 音声データを格納するバッファを確保する。
	buffer.resize(fi.audio_n * fi.audio_ch, 0);

	// FILTER_PROC_INFO を構築する。
	fpi.frame = 0;
	fpi.frame_n = fi.frame_n;
	fpi.audiop = buffer.data();
	fpi.audio_n = fi.audio_n;
	fpi.audio_ch = fi.audio_ch;
	fpi.editp = editp;

	// フレーム増加数とサンプル増加数を取得する。
	frameInc = std::max(1, fp->track[TRACK_FRAME_INC]);
	sampleInc = std::max(1, fp->track[TRACK_SAMPLE_INC]);
	sampleInc *= fi.audio_ch;
}

// 後始末を行う。
CShowWaveformApp::GetWaveform::~GetWaveform()
{
	// 終了時間を取得する。
	endTime = ::timeGetTime();

	MY_TRACE(_T("%dms\n"), endTime - startTime);

	// 経過時間を出力する。
	TCHAR text[MAX_PATH] = {};
	::StringCbPrintf(text, sizeof(text), _T("音声波形表示 - %dms"), endTime - startTime);
	::SetWindowText(theApp.m_fp->hwnd, text);

	// 拡張編集ウィンドウを再描画する。
	::InvalidateRect(theApp.Exedit_GetExeditWindow(), 0, FALSE);
}

// object の音声波形を取得する。
void CShowWaveformApp::GetWaveform::get(auls::EXEDIT_OBJECT* object)
{
	MY_TRACE_INT(object->frame_begin);
	MY_TRACE_INT(object->frame_end);

	// 音声波形オブジェクトを作成する。
	WaveformPtr waveform(new Waveform);
	theApp.m_waveformMap[object] = waveform;

	// 随時更新用にオブジェクトの状態を保存しておく。
	waveform->m_objectCopy = *object;
	waveform->m_frameBegin = object->frame_begin;
	waveform->m_frameEnd = object->frame_end;

	std::vector<auls::EXEDIT_OBJECT*> soundObjects;
	{
		// 一時的にフラグを消去する。

		int c = theApp.Exedit_GetCurrentSceneObjectCount();
		for (int i = 0; i < c; i++)
		{
			auls::EXEDIT_OBJECT* soundObject = theApp.Exedit_GetSortedObject(i);
			if (soundObject == object) continue;
			if (!(soundObject->type & auls::EXEDIT_OBJECT::TYPE_SOUND)) continue;

			soundObject->type &= ~auls::EXEDIT_OBJECT::TYPE_SOUND;
			soundObjects.push_back(soundObject);
		}
	}

	// 音声データを取得する。
	for (int frame = object->frame_begin; frame < object->frame_end - 1; frame += frameInc)
	{
		getInternal(object, waveform.get(), frame);
	}

	// 最後のフレームの音声データを取得する。
	getInternal(object, waveform.get(), object->frame_end);

	{
		// 最後のフレーム + 1 の音声データを作成する。

		int frame = object->frame_end + 1;
		short min = waveform->m_sampleArray.back().m_min;
		short max = waveform->m_sampleArray.back().m_max;
		waveform->m_sampleArray.push_back(Sample(frame - object->frame_begin, min, max));
	}

	{
		// フラグをリストアする。

		for (auto soundObject : soundObjects)
			soundObject->type |= auls::EXEDIT_OBJECT::TYPE_SOUND;
	}
}

void CShowWaveformApp::GetWaveform::getInternal(auls::EXEDIT_OBJECT* object, Waveform* waveform, int frame)
{
	// 音声データを取得するフレームを設定する。
	fpi.frame = frame;

	// バッファを初期化する。
	std::fill(buffer.begin(), buffer.end(), 0);
	// 音声データを取得する。
	BOOL result = filter->func_proc(filter, &fpi);

	short min = 0;
	short max = 0;

	// 最小値と最大値を取得する。
	for (int i = 0; i < fi.audio_n * fi.audio_ch; i += sampleInc)
	{
		for (int j = 0; j < fi.audio_ch; j++)
		{
			min = std::min(min, buffer[i + j]);
			max = std::max(max, buffer[i + j]);
		}
	}

	// 音声データを配列に追加する。
	waveform->m_sampleArray.push_back(Sample(frame - object->frame_begin, min, max));
}

void CShowWaveformApp::drawWaveform(HDC dc, LPCRECT rc)
{
	if (!m_currentDrawObject) return;

	auto it = m_waveformMap.find(m_currentDrawObject);
	if (it == m_waveformMap.end()) return;
	WaveformPtr waveform = it->second;
	if (!waveform) return;

	int scaleDiv = std::max(1, m_fp->track[TRACK_SCALE_DIV]);

	int w = rc->right - rc->left;
	int h = rc->bottom - rc->top;
	int cy = (rc->top + rc->bottom) / 2;
	int div = scaleDiv * h / 2;

	int c = waveform->m_sampleArray.size();

	std::vector<POINT> points;

	switch (m_fp->track[TRACK_SHOW_TYPE])
	{
	case 0:
		{
			// min と max を描画する。

			points.resize(c * 2);

			for (int i = 0; i < c; i++)
			{
				const Sample& sample = waveform->m_sampleArray[i];
				int x = (int)m_FrameToX(sample.m_frame + m_currentDrawObject->frame_begin);
				points[i].x = x;
				points[i].y = cy + sample.m_min / div;
				points[c * 2 - i - 1].x = x;
				points[c * 2 - i - 1].y = cy + sample.m_max / div;
			}

			break;
		}
	case 1:
		{
			// min のみを描画する。

			int c = waveform->m_sampleArray.size();

			points.resize(c + 2);

			points[0].x = rc->left;
			points[0].y = rc->bottom;
			points[c + 1].x = rc->right;
			points[c + 1].y = rc->bottom;

			for (int i = 0; i < c; i++)
			{
				const Sample& sample = waveform->m_sampleArray[i];
				int x = (int)m_FrameToX(sample.m_frame + m_currentDrawObject->frame_begin);
				points[i + 1].x = x;
				points[i + 1].y = rc->bottom + sample.m_min / div;
			}

			break;
		}
	case 2:
		{
			// max のみを描画する。

			int c = waveform->m_sampleArray.size();

			points.resize(c + 2);

			points[0].x = rc->left;
			points[0].y = rc->top;
			points[c + 1].x = rc->right;
			points[c + 1].y = rc->top;

			for (int i = 0; i < c; i++)
			{
				const Sample& sample = waveform->m_sampleArray[i];
				int x = (int)m_FrameToX(sample.m_frame + m_currentDrawObject->frame_begin);
				points[i + 1].x = x;
				points[i + 1].y = rc->top + sample.m_max / div;
			}

			break;
		}
	}

	HPEN pen = (m_penColor != CLR_NONE) ? ::CreatePen(PS_SOLID, 1, m_penColor) : (HPEN)::GetStockObject(NULL_PEN);
	HPEN oldPen = (HPEN)::SelectObject(dc, pen);
	HBRUSH brush = (m_brushColor != CLR_NONE) ? ::CreateSolidBrush(m_brushColor) : (HBRUSH)::GetStockObject(NULL_BRUSH);
	HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, brush);
	int rop2 = ::SetROP2(dc, R2_XORPEN);
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
	theApp.m_currentDrawObject = theApp.Exedit_GetObject(objectIndex);
	true_DrawObject(dc, objectIndex);
	// 描画オブジェクトをクリアする。
	theApp.m_currentDrawObject = 0;
}

BOOL WINAPI drawObjectText(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
{
	MY_TRACE(_T("drawObjectText(0x%08X, %d, %d, 0x%08X)\n"), dc, x, y, options);
	MY_TRACE_RECT2(rc[0]);
	MY_TRACE_RECT2(rc[1]);

	if (!(theApp.m_currentDrawObject->type & auls::EXEDIT_OBJECT::TYPE_SOUND))
		return ::ExtTextOut(dc, x, y, options, rc, text, c, dx);

	// フラグが立っている場合はテキストを描画する。
	if (theApp.m_fp->check[CHECK_SHOW_TEXT])
		::ExtTextOut(dc, x, y, options, rc, text, c, dx);

	// フラグが立っている場合は音声波形を描画する。
	if (theApp.m_fp->check[CHECK_SHOW_WAVEFORM])
	{
		// 描画がはみ出てもいいようにクリッピング領域を設定しておく。
		HRGN rgn = ::CreateRectRgnIndirect(rc);
		::SelectClipRgn(dc, rgn);
		::DeleteObject(rgn);

		// 音声波形を描画する。
		theApp.drawWaveform(dc, &rc[1]);

		// クリッピングを解除する。
		::SelectClipRgn(dc, 0);
	}

	return TRUE;
}

//---------------------------------------------------------------------
