#include "pch.h"
#include "Plugin.h"

//--------------------------------------------------------------------
namespace Input {
//--------------------------------------------------------------------

Plugin::Plugin(LPCTSTR fileName)
{
	MY_TRACE(_T("Plugin::Plugin()\n"));

	// プラグイン DLL をロードする。
	m_aui = ::LoadLibrary(fileName);
	MY_TRACE_HEX(m_aui);
	if (!m_aui) return;

	// プラグイン関数を取得する。
	Type_GetInputPluginTable GetInputPluginTable =
		(Type_GetInputPluginTable)::GetProcAddress(m_aui, "GetInputPluginTable");
	MY_TRACE_HEX(GetInputPluginTable);
	if (!GetInputPluginTable) return;

	// プラグインテーブルを取得する。
	m_inputPluginTable = GetInputPluginTable();
	MY_TRACE_HEX(m_inputPluginTable);
	if (!m_inputPluginTable) return;

	// プラグインを初期化する。
	if (m_inputPluginTable->func_init)
		m_inputPluginTable->func_init();
}

Plugin::~Plugin()
{
	MY_TRACE(_T("Plugin::~Plugin()\n"));

	if (!m_aui)
		return;

	// プラグインの後始末をする。
	if (m_inputPluginTable && m_inputPluginTable->func_exit)
		m_inputPluginTable->func_exit();

	// プラグイン DLL をアンロードする。
	::FreeLibrary(m_aui), m_aui = 0;
}

AviUtl::InputPluginDLL* Plugin::getInputPlugin() const
{
	return m_inputPluginTable;
}

//--------------------------------------------------------------------

Media::Media(PluginPtr plugin, LPCSTR fileName)
{
	MY_TRACE(_T("Media::Media(%hs)\n"), fileName);

	m_plugin = plugin;
	AviUtl::InputPluginDLL* ip = m_plugin->getInputPlugin();

	// メディアを開く。
	m_fileName = fileName;
	MY_TRACE_STR(fileName);

	m_inputHandle = ip->func_open(m_fileName.c_str());
	MY_TRACE_HEX(m_inputHandle);

	if (!m_inputHandle)
		return;

	// 入力情報を取得する。
	BOOL result = ip->func_info_get(m_inputHandle, &m_inputInfo);
	m_mediaInfo.flag = m_inputInfo.flag;
	m_mediaInfo.rate = m_inputInfo.rate;
	m_mediaInfo.scale = m_inputInfo.scale;
	m_mediaInfo.n = m_inputInfo.n;
	memcpy(&m_mediaInfo.format, m_inputInfo.format, sizeof(m_mediaInfo.format));
	m_mediaInfo.format.biSize = sizeof(m_mediaInfo.format);
	m_mediaInfo.format_size = sizeof(m_mediaInfo.format);
	m_mediaInfo.audio_n = m_inputInfo.audio_n;
	memcpy(&m_mediaInfo.audio_format, m_inputInfo.audio_format, sizeof(m_mediaInfo.audio_format));
	m_mediaInfo.audio_format.cbSize = sizeof(m_mediaInfo.audio_format);
	m_mediaInfo.audio_format_size = sizeof(m_mediaInfo.audio_format);
	m_mediaInfo.handler = m_inputInfo.handler;
}

Media::~Media()
{
	MY_TRACE(_T("Media::~Media()\n"));

	// メディアを閉じる。

	if (!m_inputHandle)
		return;

	AviUtl::InputPluginDLL* ip = m_plugin->getInputPlugin();

	ip->func_close(m_inputHandle);
	m_inputHandle = 0;
}

PluginPtr Media::getPlugin()
{
	return m_plugin;
}

LPCSTR Media::getFileName()
{
	return m_fileName.c_str();
}

AviUtl::InputHandle Media::getInputHandle()
{
	return m_inputHandle;
}

AviUtl::InputInfo* Media::getInputInfo()
{
	return &m_inputInfo;
}

MediaInfo* Media::getMediaInfo()
{
	return &m_mediaInfo;
}

//--------------------------------------------------------------------
} // namespace Input
//--------------------------------------------------------------------
