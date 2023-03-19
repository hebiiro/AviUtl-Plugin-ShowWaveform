#include "pch.h"
#include "Plugin.h"

//--------------------------------------------------------------------
namespace Input {
//--------------------------------------------------------------------

Plugin::Plugin()
{
	MY_TRACE(_T("Plugin::Plugin()\n"));
}

Plugin::~Plugin()
{
	MY_TRACE(_T("Plugin::~Plugin()\n"));
}

BOOL Plugin::load(LPCTSTR fileName)
{
	MY_TRACE(_T("Plugin::load(%s)\n"), fileName);

	// プラグイン DLL をロードする。
	m_aui = ::LoadLibrary(fileName);
	MY_TRACE_HEX(m_aui);
	if (!m_aui)
		return FALSE;

	// プラグイン関数を取得する。
	Type_GetInputPluginTable GetInputPluginTable =
		(Type_GetInputPluginTable)::GetProcAddress(m_aui, "GetInputPluginTable");
	MY_TRACE_HEX(GetInputPluginTable);
	if (!GetInputPluginTable)
		return FALSE;

	// プラグインテーブルを取得する。
	m_inputPluginTable = GetInputPluginTable();
	MY_TRACE_HEX(m_inputPluginTable);
	if (!m_inputPluginTable)
		return FALSE;

	// プラグインを初期化する。
	if (m_inputPluginTable->func_init)
	{
		BOOL result = m_inputPluginTable->func_init();
	}

	return TRUE;
}

BOOL Plugin::unload()
{
	MY_TRACE(_T("Plugin::unload()\n"));

	if (!m_aui)
		return FALSE;

	// プラグインの後始末をする。
	if (m_inputPluginTable && m_inputPluginTable->func_exit)
	{
		BOOL result = m_inputPluginTable->func_exit();
	}

	// プラグイン DLL をアンロードする。
	::FreeLibrary(m_aui), m_aui = 0;

	return TRUE;
}

AviUtl::InputPluginDLL* Plugin::getInputPlugin() const
{
	return m_inputPluginTable;
}

//--------------------------------------------------------------------

Media::Media()
{
	MY_TRACE(_T("Media::Media()\n"));
}

Media::~Media()
{
	MY_TRACE(_T("Media::~Media()\n"));
}

BOOL Media::open(PluginPtr plugin, LPCSTR fileName)
{
	MY_TRACE(_T("Media::open(%hs)\n"), fileName);

	m_plugin = plugin;
	AviUtl::InputPluginDLL* ip = m_plugin->getInputPlugin();

	// メディアを開く。
	m_fileName = fileName;
	MY_TRACE_STR(fileName);

	m_inputHandle = ip->func_open(m_fileName.c_str());
	MY_TRACE_HEX(m_inputHandle);

	if (!m_inputHandle)
		return FALSE;

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

	// ビデオバッファはここで確保しておく。
	int w = m_mediaInfo.format.biWidth;
	int h = m_mediaInfo.format.biHeight;
	int bytePerPixel = m_mediaInfo.format.biBitCount / 8;
	int bufferSize = w * h * bytePerPixel;

	m_videoBuffer.resize(bufferSize);

	return TRUE;
}

BOOL Media::close()
{
	MY_TRACE(_T("Media::cloes()\n"));

	// メディアを閉じる。

	if (!m_inputHandle)
		return FALSE;

	AviUtl::InputPluginDLL* ip = m_plugin->getInputPlugin();

	BOOL result = ip->func_close(m_inputHandle);
	m_inputHandle = 0;

	return result;
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

int32_t Media::calcAudioBufferSize(int32_t length)
{
	int bitsPerSample = m_mediaInfo.audio_format.wBitsPerSample;
	int channelCount = m_mediaInfo.audio_format.nChannels;
	return bitsPerSample / 8 * channelCount * length;
}

void* Media::readVideo(int32_t frame, int32_t* bufferSize)
{
	MY_TRACE(_T("Media::readVideo(%d)\n"), frame);

	AviUtl::InputPluginDLL* ip = getPlugin()->getInputPlugin();

	// 前回と同じフレームを要求すると 0 が返ってくる。
	int result = ip->func_read_video(m_inputHandle, frame, m_videoBuffer.data());
	MY_TRACE_INT(result);

	*bufferSize = m_videoBuffer.size();

	return m_videoBuffer.data();
}

void* Media::readAudio(int32_t start, int32_t length, int32_t* bufferLength)
{
	MY_TRACE(_T("Media::readAudio(%d, %d)\n"), start, length);

	{
		// オーディオバッファはここで確保する。
		int bufferSize = calcAudioBufferSize(length);
		m_audioBuffer.resize(bufferSize);
	}

	AviUtl::InputPluginDLL* ip = getPlugin()->getInputPlugin();

	*bufferLength = ip->func_read_audio(m_inputHandle, start, length, m_audioBuffer.data());
	MY_TRACE_INT(*bufferLength);

	return m_audioBuffer.data();
}

//--------------------------------------------------------------------
} // namespace Input
//--------------------------------------------------------------------
