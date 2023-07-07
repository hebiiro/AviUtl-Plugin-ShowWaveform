#pragma once

//--------------------------------------------------------------------

static const LPCTSTR PROP_NAME_WINDOW_CONTAINER = _T("WindowContainer");
static const LPCTSTR PROP_NAME_DIALOG_CONTAINER = _T("DialogContainer");

// OutProcess => InProcess
static const UINT WM_AVIUTL_FILTER_INIT = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_INIT"));
static const UINT WM_AVIUTL_FILTER_RECEIVE = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_RECEIVE"));
static const UINT WM_AVIUTL_FILTER_CHANGE_FRAME = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_CHANGE_FRAME"));

// InProcess => OutProcess
static const UINT WM_AVIUTL_FILTER_EXIT = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_EXIT"));
static const UINT WM_AVIUTL_FILTER_RESIZE = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_RESIZE"));
static const UINT WM_AVIUTL_FILTER_SEND = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_SEND"));
static const UINT WM_AVIUTL_FILTER_CLEAR = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_CLEAR"));
static const UINT WM_AVIUTL_FILTER_REDRAW = ::RegisterWindowMessage(_T("WM_AVIUTL_FILTER_REDRAW"));

struct SendID {
	static const int requestCache = 1;
	static const int notifyProjectChanged = 2;
	static const int notifyItemChanged = 3;
	static const int notifyFullSamplesChanged = 4;
};

//--------------------------------------------------------------------

static const int32_t SAMPLE_FPS = 30;
static const int32_t MAX_SAMPLE_COUNT = SAMPLE_FPS * 60 * 60 * 4; // 最大 4 時間

struct Sample {
	float level;
};

struct SenderBottle {
	char fileName[MAX_PATH];
};

struct ReceiverBottle {
	char fileName[MAX_PATH];
	int32_t sampleCount;
	Sample samples[MAX_SAMPLE_COUNT];
};

struct ReaderBottle {
	char fileName[MAX_PATH];
	int32_t sampleCount;
	Sample samples[MAX_SAMPLE_COUNT];
};

struct ProjectParams {
	int32_t video_scale;
	int32_t video_rate;
	int32_t frameNumber;
	int32_t sceneIndex;
	int32_t currentFrame;
};

struct AudioParams {
	uint32_t id = 0;
	uint32_t flag = 0;
	int32_t frameBegin = 0;
	int32_t frameEnd = 0;
	int32_t sceneSet = 0;
	char fileName[MAX_PATH] = {};
	float volume = 1.0;
	int32_t playBegin = 0;
	float playSpeed = 1.0;
	uint32_t layerFlag = 0;
};

struct FullSamplesParams {
	BOOL showBPM = FALSE; // BPM を表示するかどうか。
	struct Tempo {
		int32_t orig = 0; // テンポの基準となるフレーム番号。
		int32_t bpm = 120; // 1 分あたりの四分音符の数。テンポの速さはこの変数だけで決まる。
		int32_t above = 4; // 楽譜に書いてある上の数字。拍子。テンポの速さには影響を与えない。
		int32_t below = 4; // 楽譜に書いてある下の数字。分。テンポの速さには影響を与えない。
	} tempo;
};

//--------------------------------------------------------------------

inline FormatText getMutexName(HWND hwnd)
{
	return FormatText(_T("ShowWaveform.Mutex.%08X"), hwnd);
}

inline FormatText getSharedSenderBottleName(HWND hwnd)
{
	return FormatText(_T("ShowWaveform.Shared.SenderBottle.%08X"), hwnd);
}

inline FormatText getSharedReceiverBottleName(HWND hwnd)
{
	return FormatText(_T("ShowWaveform.Shared.ReceiverBottle.%08X"), hwnd);
}

inline FormatText getReaderEventName(DWORD id)
{
	return FormatText(_T("ShowWaveform.Event.Reader.%d"), id);
}

inline FormatText getSharedReaderBottleName(DWORD id)
{
	return FormatText(_T("ShowWaveform.Shared.ReaderBottle.%d"), id);
}

inline FormatText getSharedProjectParamsName(HWND hwnd)
{
	return FormatText(_T("ShowWaveform.Shared.ProjectParams.%08X"), hwnd);
}

inline FormatText getSharedAudioParamsName(HWND hwnd)
{
	return FormatText(_T("ShowWaveform.Shared.AudioParams.%08X"), hwnd);
}

inline FormatText getSharedSenderFullSamplesParamsName(HWND hwnd)
{
	return FormatText(_T("ShowWaveform.Shared.SenderFullSamplesParams.%08X"), hwnd);
}

inline FormatText getSharedReceiverFullSamplesParamsName(HWND hwnd)
{
	return FormatText(_T("ShowWaveform.Shared.ReceiverFullSamplesParams.%08X"), hwnd);
}

//--------------------------------------------------------------------

typedef std::shared_ptr<SenderBottle> SenderBottlePtr;
typedef std::shared_ptr<ReceiverBottle> ReceiverBottlePtr;
typedef std::shared_ptr<ProjectParams> ProjectParamsPtr;
typedef std::shared_ptr<AudioParams> AudioParamsPtr;
typedef std::shared_ptr<FullSamplesParams> FullSamplesParamsPtr;

//--------------------------------------------------------------------

struct Shared {
	Mutex mutex;
	SimpleFileMappingT<SenderBottle> senderBottle;
	SimpleFileMappingT<ReceiverBottle> receiverBottle;
	SimpleFileMappingT<ProjectParams> projectParams;
	SimpleFileMappingT<AudioParams> audioParams;
	SimpleFileMappingT<FullSamplesParams> senderFullSamplesParams;
	SimpleFileMappingT<FullSamplesParams> receiverFullSamplesParams;

	BOOL init(HWND hwnd)
	{
		MY_TRACE(_T("Shared::init()\n"));

		mutex.init(0, FALSE, getMutexName(hwnd));
		senderBottle.init(getSharedSenderBottleName(hwnd));
		receiverBottle.init(getSharedReceiverBottleName(hwnd));
		projectParams.init(getSharedProjectParamsName(hwnd));
		audioParams.init(getSharedAudioParamsName(hwnd));
		senderFullSamplesParams.init(getSharedSenderFullSamplesParamsName(hwnd));
		receiverFullSamplesParams.init(getSharedReceiverFullSamplesParamsName(hwnd));

		return TRUE;
	}

	BOOL term()
	{
		MY_TRACE(_T("Shared::term()\n"));

		return TRUE;
	}

	SenderBottlePtr getSenderBottle()
	{
		SenderBottle* shared = senderBottle.getBuffer();
		if (!shared) return 0;
		return std::make_shared<SenderBottle>(*shared);
	}

	ProjectParamsPtr getProjectParams()
	{
		MY_TRACE(_T("Shared::getProjectParams()\n"));

		ProjectParams* shared = projectParams.getBuffer();
		if (!shared) return 0;
		return std::make_shared<ProjectParams>(*shared);
	}

	AudioParamsPtr getAudioParams()
	{
		MY_TRACE(_T("Shared::getAudioParams()\n"));

		AudioParams* shared = audioParams.getBuffer();
		if (!shared) return 0;
		return std::make_shared<AudioParams>(*shared);
	}

	FullSamplesParamsPtr getSenderFullSamplesParams()
	{
		MY_TRACE(_T("Shared::getSenderFullSamplesParams()\n"));

		FullSamplesParams* shared = senderFullSamplesParams.getBuffer();
		if (!shared) return 0;
		return std::make_shared<FullSamplesParams>(*shared);
	}

	BOOL setSenderFullSamplesParams(const FullSamplesParams* params)
	{
		MY_TRACE(_T("Shared::setSenderFullSamplesParams()\n"));

		Synchronizer sync(mutex);
		FullSamplesParams* shared = senderFullSamplesParams.getBuffer();
		if (!shared) return FALSE;
		*shared = *params;
		return TRUE;
	}

	FullSamplesParamsPtr getReceiverFullSamplesParams()
	{
		MY_TRACE(_T("Shared::getReceiverFullSamplesParams()\n"));

		FullSamplesParams* shared = receiverFullSamplesParams.getBuffer();
		if (!shared) return 0;
		return std::make_shared<FullSamplesParams>(*shared);
	}

	BOOL setReceiverFullSamplesParams(const FullSamplesParams* params)
	{
		MY_TRACE(_T("Shared::setReceiverFullSamplesParams()\n"));

		Synchronizer sync(mutex);
		FullSamplesParams* shared = receiverFullSamplesParams.getBuffer();
		if (!shared) return FALSE;
		*shared = *params;
		return TRUE;
	}
};

//--------------------------------------------------------------------
