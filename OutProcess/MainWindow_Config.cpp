#include "pch.h"
#include "MainWindow.h"
#include "App.h"

//--------------------------------------------------------------------

const Label MainWindow::HorzScaleSettings::VisibleStyle::labels[] = {
	{ both, L"both" },
	{ top, L"top" },
	{ bottom, L"bottom" },
};

const Label MainWindow::HorzScaleSettings::LineStyle::labels[] = {
	{ side, L"side" },
	{ straight, L"straight" },
};

const Label MainWindow::VertScaleSettings::VisibleStyle::labels[] = {
	{ both, L"both" },
	{ left, L"left" },
	{ right, L"right" },
};

const Label MainWindow::VertScaleSettings::LineStyle::labels[] = {
	{ side, L"side" },
	{ straight, L"straight" },
};

const Label MainWindow::Mode::labels[] = {
	{ rms, L"rms" },
	{ center, L"center" },
	{ bottomUp, L"bottomUp" },
	{ topDown, L"topDown" },
};

//--------------------------------------------------------------------

BOOL MainWindow::initConfig()
{
	MY_TRACE(_T("MainWindow::initConfig()\n"));

	WCHAR fileName[MAX_PATH] = {};
	::GetModuleFileNameW(theApp.instance, fileName, MAX_PATH);
	::PathRemoveFileSpecW(fileName);
	::PathAppendW(fileName, _T("Config.ini"));
	MY_TRACE_WSTR(fileName);

	m_configFileChecker = std::make_shared<FileUpdateChecker>(fileName);
	::SetTimer(m_hwnd, TimerID::checkConfig, 1000, 0); // 1 秒に 1 回コンフィグファイルをチェックする。
	reloadConfig();

	return TRUE;
}

BOOL MainWindow::termConfig()
{
	MY_TRACE(_T("MainWindow::termConfig()\n"));

	::KillTimer(m_hwnd, TimerID::checkConfig);

	save(m_configFileChecker->getFilePath());

	return TRUE;
}

void MainWindow::reloadConfig()
{
	MY_TRACE(_T("MainWindow::reloadConfig()\n"));

	load(m_configFileChecker->getFilePath());

	recalcConfig();
}

void MainWindow::recalcConfig()
{
	MY_TRACE(_T("MainWindow::recalcConfig()\n"));

	ClientDC dc(m_hwnd);
	MakeCurrent makeCurrent(dc, m_rc);

	recalcConfigInternal();
}

void MainWindow::recalcConfigInternal()
{
	MY_TRACE(_T("MainWindow::recalcConfigInternal()\n"));

	MY_TRACE_WSTR((BSTR)g_design.fontDefault);
	m_fontDefault = nvgCreateFont(m_vg, "default", StringU8(g_design.fontDefault));
	MY_TRACE_INT(m_fontDefault);

	MY_TRACE_WSTR((BSTR)g_design.fontDefault2);
	m_fontDefault2 = nvgCreateFont(m_vg, "default2", StringU8(g_design.fontDefault2));
	MY_TRACE_INT(m_fontDefault2);

	nvgAddFallbackFontId(m_vg, m_fontDefault, m_fontDefault2);

	if (m_image) nvgDeleteImage(m_vg, m_image);
	m_image = nvgCreateImage(m_vg, g_design.image.fileName, NVG_IMAGE_NEAREST);
	MY_TRACE_INT(m_image);
}

LRESULT MainWindow::onTimerCheckConfig(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_configFileChecker->isFileUpdated())
	{
		reloadConfig();
		redraw();
	}

	return 0;
}

void MainWindow::load(LPCWSTR fileName)
{
	MY_TRACE(_T("MainWindow::load(%ws)\n"), fileName);

	int mode = m_mode->getID();

	getPrivateProfileInt(fileName, L"Config", L"maxReaderCount", theApp.readerManager.maxReaderCount);
	getPrivateProfileInt(fileName, L"Config", L"limitVolume", m_limitVolume);
	getPrivateProfileInt(fileName, L"Config", L"baseVolume", m_baseVolume);
	getPrivateProfileInt(fileName, L"Config", L"minRMS", m_minRMS);
	getPrivateProfileInt(fileName, L"Config", L"maxRMS", m_maxRMS);
	getPrivateProfileInt(fileName, L"Config", L"baseRMS", m_baseRMS);
	getPrivateProfileInt(fileName, L"Config", L"zoom", m_zoom);
	getPrivateProfileLabel(fileName, L"Config", L"mode", mode, Mode::labels);
	getPrivateProfileLabel(fileName, L"Config", L"horzScaleSettings.visibleStyle", m_horzScaleSettings.visibleStyle, HorzScaleSettings::VisibleStyle::labels);
	getPrivateProfileLabel(fileName, L"Config", L"horzScaleSettings.lineStyle", m_horzScaleSettings.lineStyle, HorzScaleSettings::LineStyle::labels);
	getPrivateProfileLabel(fileName, L"Config", L"vertScaleSettings.visibleStyle", m_vertScaleSettings.visibleStyle, VertScaleSettings::VisibleStyle::labels);
	getPrivateProfileLabel(fileName, L"Config", L"vertScaleSettings.lineStyle", m_vertScaleSettings.lineStyle, VertScaleSettings::LineStyle::labels);

	switch (mode)
	{
	case Mode::rms: m_mode = std::make_shared<RMSMode>(); break;
	case Mode::center: m_mode = std::make_shared<CenterMode>(); break;
	case Mode::bottomUp: m_mode = std::make_shared<BottomUpMode>(); break;
	case Mode::topDown: m_mode = std::make_shared<TopDownMode>(); break;
	}

	g_design.load(fileName);
}

void MainWindow::save(LPCWSTR fileName)
{
	MY_TRACE(_T("MainWindow::save(%ws)\n"), fileName);

	int mode = m_mode->getID();

	setPrivateProfileInt(fileName, L"Config", L"maxReaderCount", theApp.readerManager.maxReaderCount);
	setPrivateProfileInt(fileName, L"Config", L"limitVolume", m_limitVolume);
	setPrivateProfileInt(fileName, L"Config", L"baseVolume", m_baseVolume);
	setPrivateProfileInt(fileName, L"Config", L"minRMS", m_minRMS);
	setPrivateProfileInt(fileName, L"Config", L"maxRMS", m_maxRMS);
	setPrivateProfileInt(fileName, L"Config", L"baseRMS", m_baseRMS);
	setPrivateProfileInt(fileName, L"Config", L"zoom", m_zoom);
	setPrivateProfileLabel(fileName, L"Config", L"mode", mode, Mode::labels);
	setPrivateProfileLabel(fileName, L"Config", L"horzScaleSettings.visibleStyle", m_horzScaleSettings.visibleStyle, HorzScaleSettings::VisibleStyle::labels);
	setPrivateProfileLabel(fileName, L"Config", L"horzScaleSettings.lineStyle", m_horzScaleSettings.lineStyle, HorzScaleSettings::LineStyle::labels);
	setPrivateProfileLabel(fileName, L"Config", L"vertScaleSettings.visibleStyle", m_vertScaleSettings.visibleStyle, VertScaleSettings::VisibleStyle::labels);
	setPrivateProfileLabel(fileName, L"Config", L"vertScaleSettings.lineStyle", m_vertScaleSettings.lineStyle, VertScaleSettings::LineStyle::labels);

	g_design.save(fileName);
}

//--------------------------------------------------------------------
