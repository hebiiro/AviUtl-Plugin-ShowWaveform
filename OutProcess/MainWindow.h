#pragma once

#include "OutProcess.h"
#include "Design.h"
#include "Mode.h"

//--------------------------------------------------------------------

struct MakeCurrent
{
	HDC m_prevDC = 0;
	HGLRC m_prevRC = 0;

	MakeCurrent(HDC dc, HGLRC rc)
	{
		m_prevDC = wglGetCurrentDC();
		m_prevRC = wglGetCurrentContext();

		wglMakeCurrent(dc, rc);
	}

	~MakeCurrent()
	{
		wglMakeCurrent(m_prevDC, m_prevRC);
	}
};

//--------------------------------------------------------------------

typedef std::shared_ptr<SenderBottle> SenderBottlePtr;
typedef std::shared_ptr<ReceiverBottle> ReceiverBottlePtr;
typedef std::shared_ptr<ProjectParams> ProjectParamsPtr;
typedef std::shared_ptr<AudioParams> AudioParamsPtr;
typedef std::map<uint32_t, AudioParamsPtr> AudioParamsMap;

struct Cache {
	std::string fileName;
	std::vector<Sample> samples;
};

typedef std::shared_ptr<Cache> CachePtr;
typedef std::map<std::string, CachePtr> CacheMap;

struct Waiter {
	std::string fileName;
	Waiter(LPCSTR fileName);
};

typedef std::shared_ptr<Waiter> WaiterPtr;
typedef std::deque<WaiterPtr> WaiterQueue;

struct Reader {
	PROCESS_INFORMATION pi = {};
	SimpleFileMappingT<ReaderBottle> shared;
	Reader(HWND hwnd);
	~Reader();
	DWORD getId();
	ReaderBottle* getBottle();
};

typedef std::shared_ptr<Reader> ReaderPtr;
typedef std::map<DWORD, ReaderPtr> ReaderMap;

struct FullSample {
	float level;
	float rms;
};

typedef std::vector<FullSample> FullSamples;

//--------------------------------------------------------------------

struct MainWindow
{
	struct CommandID {
		static const UINT rmsMode = 1000;
		static const UINT bothMode = 1001;
		static const UINT minMode = 1002;
		static const UINT maxMode = 1003;
	};

	struct TimerID {
		static const UINT checkConfig = 1000;
	};

	HGLRC m_rc = 0;
	NVGcontext* m_vg = 0;
	int m_fontDefault = 0;
	int m_fontDefault2 = 0;
	int m_image = 0;

	int m_minRange = -33;
	int m_maxRange = 14;
	int m_baseLevel = 0;
	int m_zoom = 0;
	int m_scale = 0;

	int m_hotFrame = 0;

	struct Mode {
		static const int rms = 0;
		static const int both = 1;
		static const int min = 2;
		static const int max = 3;
		static const Label labels[];
	};
	ModePtr m_mode = std::make_shared<RMSMode>();

	void recalcLayout();
	int px2Frame(int x);

	FileUpdateCheckerPtr m_configFileChecker;

	BOOL initConfig();
	BOOL termConfig();
	void reloadConfig();
	void recalcConfig();
	void recalcConfigInternal();
	LRESULT onTimerCheckConfig(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	void load(LPCWSTR fileName);
	void save(LPCWSTR fileName);

	static BOOL setupPixelFormat(HDC dc);
	BOOL initOpenGL();
	BOOL termOpenGL();
	void doPaint(HDC dc, const RECT& rc);
	void doPaint(const RECT& rc);

	HWND m_hwnd = 0;
	SimpleFileMappingT<SenderBottle> m_sharedSenderBottle;
	SimpleFileMappingT<ReceiverBottle> m_sharedReceiverBottle;
	SimpleFileMappingT<ProjectParams> m_sharedProjectParams;
	SimpleFileMappingT<AudioParams> m_sharedAudioParams;
	CacheMap cacheMap;

	BOOL initWaveform();
	BOOL termWaveform();

	SenderBottlePtr getSenderBottle();
	CachePtr getCache(LPCSTR fileName);

	FullSamples fullSamples;

	void recalcFullSamples();

	ProjectParamsPtr projectParams;
	ProjectParamsPtr getProjectParams();
	void setProjectParams(const ProjectParamsPtr& projectParams);

	AudioParamsMap audioParamsMap;
	AudioParamsPtr getAudioParams();
	void setAudioParams(const AudioParamsPtr& projectParams);

	WaiterQueue waiterQueue;

	WaiterPtr createWaiter(LPCSTR fileName);
	void digestWaiterQueue();

	ReaderMap readerMap;
	int maxReaderCount = 1;

	ReaderPtr getReader(DWORD id);
	ReaderPtr createReader(LPCSTR fileName);
	void eraseReader(DWORD id);
	CachePtr createCache(const ReaderPtr& reader);
	void sendCache(const CachePtr& cache);

	LRESULT onCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onDestroy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onSize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onHScroll(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onTimer(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onPaint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onContextMenu(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onSetCursor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onMouseMove(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onMouseWheel(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onLButtonDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onLButtonUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterExit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterResize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterSend(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterReceive(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterClear(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterRedraw(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

 extern MainWindow g_mainWindow;

//--------------------------------------------------------------------
