#include "pch.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

const Label MainWindow::Mode::labels[] = {
	{ rms, L"rms" },
	{ both, L"both" },
	{ min, L"min" },
	{ max, L"max" },
};

//--------------------------------------------------------------------

BOOL MainWindow::initConfig()
{
	MY_TRACE(_T("MainWindow::initConfig()\n"));

	WCHAR fileName[MAX_PATH] = {};
	::GetModuleFileNameW(g_instance, fileName, MAX_PATH);
	::PathRemoveFileSpecW(fileName);
	::PathAppendW(fileName, _T("Config.ini"));
	MY_TRACE_WSTR(fileName);

	m_configFileChecker = std::make_shared<FileUpdateChecker>(fileName);
	::SetTimer(m_hwnd, TimerID::checkConfig, 1000, 0);
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
	m_fontDefault = nvgCreateFont(m_vg, "default", StringU8(g_design.fontDefault));
	m_fontDefault2 = nvgCreateFont(m_vg, "default2", StringU8(g_design.fontDefault2));
	nvgAddFallbackFontId(m_vg, m_fontDefault, m_fontDefault2);

	if (m_image)
		nvgDeleteImage(m_vg, m_image);

	m_image = nvgCreateImage(m_vg, g_design.image.fileName, 0);
}

LRESULT MainWindow::onTimerCheckConfig(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_configFileChecker->isFileUpdated())
	{
		reloadConfig();

		::InvalidateRect(hwnd, 0, FALSE);
	}

	return 0;
}

void MainWindow::load(LPCWSTR fileName)
{
	MY_TRACE(_T("MainWindow::load(%ws)\n"), fileName);

	int mode = m_mode->getID();

	getPrivateProfileInt(fileName, L"Config", L"minRange", m_minRange);
	getPrivateProfileInt(fileName, L"Config", L"maxRange", m_maxRange);
	getPrivateProfileInt(fileName, L"Config", L"baseLevel", m_baseLevel);
	getPrivateProfileInt(fileName, L"Config", L"maxReaderCount", maxReaderCount);
	getPrivateProfileInt(fileName, L"Config", L"zoom", m_zoom);
	getPrivateProfileInt(fileName, L"Config", L"scale", m_scale);
	getPrivateProfileLabel(fileName, L"Config", L"mode", mode, Mode::labels);

	switch (mode)
	{
	case Mode::rms: m_mode = std::make_shared<RMSMode>(); break;
	case Mode::both: m_mode = std::make_shared<BothMode>(); break;
	case Mode::min: m_mode = std::make_shared<MinMode>(); break;
	case Mode::max: m_mode = std::make_shared<MaxMode>(); break;
	}

	g_design.load(fileName);
}

void MainWindow::save(LPCWSTR fileName)
{
	MY_TRACE(_T("MainWindow::save(%ws)\n"), fileName);

	int mode = m_mode->getID();

	setPrivateProfileInt(fileName, L"Config", L"minRange", m_minRange);
	setPrivateProfileInt(fileName, L"Config", L"maxRange", m_maxRange);
	setPrivateProfileInt(fileName, L"Config", L"baseLevel", m_baseLevel);
	setPrivateProfileInt(fileName, L"Config", L"maxReaderCount", maxReaderCount);
	setPrivateProfileInt(fileName, L"Config", L"zoom", m_zoom);
	setPrivateProfileInt(fileName, L"Config", L"scale", m_scale);
	setPrivateProfileLabel(fileName, L"Config", L"mode", mode, Mode::labels);

	g_design.save(fileName);
}

//--------------------------------------------------------------------
