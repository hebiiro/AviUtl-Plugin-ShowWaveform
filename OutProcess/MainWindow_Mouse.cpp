#include "pch.h"
#include "MainWindow.h"
#include "App.h"

//--------------------------------------------------------------------

LRESULT MainWindow::onSetCursor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	MY_TRACE(_T("MainWindow::onSetCursor(0x%08X, 0x%08X)\n"), wParam, lParam);

	HWND cursorHolder = (HWND)wParam;

	if (cursorHolder == hwnd && LOWORD(lParam) == HTCLIENT)
	{
		POINT point; ::GetCursorPos(&point);
		::MapWindowPoints(0, hwnd, &point, 1);

		int ht = hitTest(point);

		switch (ht)
		{
		case HitTest::HorzScale: ::SetCursor(::LoadCursor(0, IDC_SIZEWE)); return TRUE;
		case HitTest::VertScaleMin:
		case HitTest::VertScaleMax: ::SetCursor(::LoadCursor(0, IDC_SIZENS)); return TRUE;
		}
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onMouseMove(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	MY_TRACE(_T("MainWindow::onMouseMove(0x%08X, 0x%08X)\n"), wParam, lParam);

	POINT point = LP2PT(lParam);

	if (::GetCapture() == hwnd)
	{
		// ※ダイアログも更新しないといけないので App のセッターを使用する。

		switch (drag.ht)
		{
		case MainWindow::HitTest::HorzScale:
			{
				// zoom をドラッグして変更する。

				int offset = point.x - drag.origPoint.x;

				theApp.setZoom(drag.origValue + offset);

				break;
			}
		case MainWindow::HitTest::VertScaleMin:
		case MainWindow::HitTest::VertScaleMax:
			{
				// minRMS, maxRMS, limitVolume をドラッグして変更する。

				int offset = (point.y - drag.origPoint.y) / 2;

				switch (getMode()->getID())
				{
				case Mode::rms:
					{
						if (drag.ht == MainWindow::HitTest::VertScaleMin)
							theApp.setMinRMS(drag.origValue + offset);
						else
							theApp.setMaxRMS(drag.origValue + offset);

						break;
					}
				case Mode::center:
					{
						if (drag.ht == MainWindow::HitTest::VertScaleMin)
							theApp.setLimitVolume(drag.origValue - offset);
						else
							theApp.setLimitVolume(drag.origValue + offset);

						break;
					}
				case Mode::bottomUp:
					{
						theApp.setLimitVolume(drag.origValue + offset);

						break;
					}
				case Mode::topDown:
					{
						theApp.setLimitVolume(drag.origValue - offset);

						break;
					}
				}

				break;
			}
		}
	}
	else
	{
//		MY_TRACE(_T("ホットフレームを更新します\n"));

		int newHotFrame = client2frame(point.x);

		setHotFrame(newHotFrame);
		outputFrames();
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onMouseWheel(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int delta = (short)HIWORD(wParam);
	POINT point = LP2PT(lParam);
	::MapWindowPoints(0, hwnd, &point, 1);

	MY_TRACE(_T("MainWindow::onMouseWheel(%d, %d, %d)\n"), delta, point.x, point.y);

	int newZoom = getZoom();

	if (delta > 0)
	{
		newZoom += 10;
	}
	else
	{
		newZoom -= 10;
	}

	theApp.setZoom(newZoom);

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onLButtonDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onLButtonDown(0x%08X, 0x%08X)\n"), wParam, lParam);

	POINT point = LP2PT(lParam);

	drag.ht = hitTest(point);

	switch (drag.ht)
	{
	case HitTest::HorzScale:
		{
			// zoom のドラッグを開始する。

			::SetCapture(hwnd);
			drag.origPoint = point;
			drag.origValue = getZoom();

			break;
		}
	case HitTest::VertScaleMin:
		{
			// minRMS, limitVolume をドラッグを開始する。

			::SetCapture(hwnd);
			drag.origPoint = point;
			drag.origValue = (getMode()->getID() == Mode::rms) ? getMinRMS() : getLimitVolume();

			break;
		}
	case HitTest::VertScaleMax:
		{
			// maxRMS, limitVolume をドラッグを開始する。

			::SetCapture(hwnd);
			drag.origPoint = point;
			drag.origValue = (getMode()->getID() == Mode::rms) ? getMaxRMS() : getLimitVolume();

			break;
		}
	default:
		{
			// カレントフレームを変更する。

			int32_t frame = client2frame(point.x);

			::PostMessage(theApp.windowContainer, WM_AVIUTL_FILTER_CHANGE_FRAME, (WPARAM)frame, 0);

			break;
		}
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT MainWindow::onLButtonUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("MainWindow::onLButtonUp(0x%08X, 0x%08X)\n"), wParam, lParam);

	::ReleaseCapture();

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//--------------------------------------------------------------------
