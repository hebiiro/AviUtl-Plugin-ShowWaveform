#pragma once

#include "OutProcess.h"
#include "Plugin.h"
#include "Design.h"

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

typedef std::shared_ptr<Bottle> BottlePtr;
typedef std::shared_ptr<ProjectParams> ProjectParamsPtr;
typedef std::shared_ptr<AudioParams> AudioParamsPtr;
typedef std::map<uint32_t, AudioParamsPtr> AudioParamsMap;

struct Cache {
	std::string fileName;
	std::vector<Sample> samples;
};

typedef std::shared_ptr<Cache> CachePtr;
typedef std::map<std::string, CachePtr> CacheMap;

//--------------------------------------------------------------------

struct MainWindow
{
	struct TimerID {
		static const UINT checkConfig = 1000;
	};

	struct DragMode {
		static const int minRange = 0;
		static const int maxRange = 1;
	};

	HGLRC m_rc = 0;
	NVGcontext* m_vg = 0;
	int m_fontDefault = 0;
	int m_fontDefault2 = 0;
	int m_image = 0;

	int m_minRange = -33;
	int m_maxRange = 14;
	int m_baseLevel = 0;

	int m_dragMode = DragMode::minRange;
	POINT m_dragOriginPoint = {};
	int m_dragOriginRange = 0;
	int m_hotFrame = 0;

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
	struct PaintContext {
		RECT rc;
		int width, height;
		float x, y, w, h;
	};
	void drawBackground(const PaintContext& context);
	void drawScale(const PaintContext& context);
	void drawBody(const PaintContext& context);
	void drawGraph(const PaintContext& context);
	void drawText(float x, float y, LPCSTR text, const Design::Text& design);

	HWND m_hwnd = 0;
	Mutex m_mutex;
	SimpleFileMapping m_fileMapping;
	SimpleFileMapping m_fileMappingProjectParams;
	SimpleFileMapping m_fileMappingAudioParams;
	CacheMap cacheMap;
	Input::PluginPtr plugin;

	BOOL initWaveform();
	BOOL termWaveform();

	BottlePtr getBottle();
	CachePtr getCache(const BottlePtr& bottle);
	CachePtr createCache(const BottlePtr& bottle);
	void getSample(const CachePtr& cache);
	void fireReceive(const CachePtr& cache);

	static float normalize(char pcm);
	static float normalize(short pcm);
	static float normalize(long pcm);
	static float normalize(float pcm);
	template<typename T>
	static float calc(const T* samples, int count);
	static float normalize24(long pcm);
	static long convert24(const BYTE* sample);
	static float calc24(const BYTE* samples, int count);

	std::vector<float> fullSamples;
	void recalcWaveform();

	ProjectParamsPtr projectParams;
	ProjectParamsPtr getProjectParams();
	void setProjectParams(const ProjectParamsPtr& projectParams);

	AudioParamsMap audioParamsMap;
	AudioParamsPtr getAudioParams();
	void setAudioParams(const AudioParamsPtr& projectParams);

	LRESULT onCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onDestroy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onTimer(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onPaint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onSetCursor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onMouseMove(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onLButtonDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onLButtonUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterExit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterResize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterSend(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterClear(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onAviUtlFilterRedraw(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

 extern MainWindow g_mainWindow;

//--------------------------------------------------------------------
