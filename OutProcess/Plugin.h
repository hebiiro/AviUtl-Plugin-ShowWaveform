#pragma once

//--------------------------------------------------------------------

struct MediaInfo
{
	AviUtl::InputInfo::Flag
						flag;				//	フラグ
	int32_t				rate,scale;			//	フレームレート
	int32_t				n;					//	フレーム数
	BITMAPINFOHEADER	format;				//	画像フォーマットへのポインタ(次に関数が呼ばれるまで内容を有効にしておく)
	int32_t				format_size;		//	画像フォーマットのサイズ
	int32_t				audio_n;			//	音声サンプル数
	WAVEFORMATEX		audio_format;		//	音声フォーマットへのポインタ(次に関数が呼ばれるまで内容を有効にしておく)
	int32_t				audio_format_size;	//	音声フォーマットのサイズ
	DWORD				handler;			//	画像codecハンドラ
};

//--------------------------------------------------------------------
namespace Input {
//--------------------------------------------------------------------

class Plugin;
class Media;

typedef std::shared_ptr<Plugin> PluginPtr;
typedef std::shared_ptr<Media> MediaPtr;

//--------------------------------------------------------------------

typedef AviUtl::InputPluginDLL* (WINAPI* Type_GetInputPluginTable)();

class Plugin
{
public:

	HINSTANCE m_aui = 0;
	AviUtl::InputPluginDLL* m_inputPluginTable = 0;

public:

	Plugin();
	~Plugin();

	BOOL load(LPCTSTR fileName);
	BOOL unload();

	AviUtl::InputPluginDLL* getInputPlugin() const;
};

//--------------------------------------------------------------------

class Media
{
private:

	PluginPtr m_plugin;
	std::string m_fileName;
	AviUtl::InputHandle m_inputHandle = 0;
	AviUtl::InputInfo m_inputInfo = {};
	MediaInfo m_mediaInfo = {};
	std::vector<BYTE> m_videoBuffer;
	std::vector<BYTE> m_audioBuffer;

public:

	Media();
	~Media();

	BOOL open(PluginPtr plugin, LPCSTR fileName);
	BOOL close();

	PluginPtr getPlugin();
	LPCSTR getFileName();
	AviUtl::InputHandle getInputHandle();
	AviUtl::InputInfo* getInputInfo();
	MediaInfo* getMediaInfo();
	int32_t calcAudioBufferSize(int32_t length);
	void* readVideo(int32_t frame, int32_t* bufferSize);
	void* readAudio(int32_t start, int32_t length, int32_t* bufferLength);
};

//--------------------------------------------------------------------
} // namespace Input
//--------------------------------------------------------------------
