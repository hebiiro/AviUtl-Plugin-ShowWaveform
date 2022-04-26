#pragma once

//---------------------------------------------------------------------
// Api Hook

DECLARE_HOOK_PROC(void, CDECL, DrawObject, (HDC dc, int objectIndex));

BOOL WINAPI drawObjectText(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx);

//---------------------------------------------------------------------

extern int TRACK_FRAME_INC;
extern int TRACK_SAMPLE_INC;
extern int TRACK_SCALE_DIV;
extern int TRACK_SHOW_TYPE;
extern int TRACK_UPDATE_MODE;

extern int CHECK_UPDATE_SELECTED_ITEM;
extern int CHECK_UPDATE_ALL_ITEMS;
extern int CHECK_DELETE_SELECTED_ITEM;
extern int CHECK_DELETE_ALL_ITEMS;
extern int CHECK_SHOW_WAVEFORM;
extern int CHECK_SHOW_TEXT;

//---------------------------------------------------------------------

class CShowWaveformApp
{
public:

	struct Sample
	{
		Sample(int frame, short min, short max)
			: m_frame(frame)
			, m_min(min)
			, m_max(max)
		{
		}

		int m_frame;
		short m_min;
		short m_max;
	};

	typedef std::vector<Sample> SampleArray;

	struct Waveform
	{
		SampleArray m_sampleArray;
		ExEdit::Object m_objectCopy = {};
		int m_frameBegin = 0;
		int m_frameEnd = 0;
	};

	typedef std::shared_ptr<Waveform> WaveformPtr;
	typedef std::map<ExEdit::Object*, WaveformPtr> WaveformMap;

	struct GetWaveform
	{
		DWORD startTime = 0;
		DWORD endTime = 0;

		FILTER* filter = 0;
		void* editp = 0;
		FILE_INFO fi = {};
		std::vector<short> buffer;
		FILTER_PROC_INFO fpi = {};
		int frameInc = 0;
		int sampleInc = 0;

		GetWaveform(FILTER *fp);
		~GetWaveform();
		void get(ExEdit::Object* object);
		void getInternal(ExEdit::Object* object, Waveform* waveform, int frame);
	};

public:

	AviUtlInternal m_auin;

	HINSTANCE m_instance;
	FILTER* m_fp;
	ExEdit::Object* m_currentDrawObject;
	WaveformMap m_waveformMap;

	COLORREF m_penColor;
	COLORREF m_brushColor;

public:

	CShowWaveformApp();
	~CShowWaveformApp();

	BOOL DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved);
	void load(int* track_def);
	BOOL func_init(FILTER *fp);
	BOOL func_exit(FILTER *fp);
	BOOL func_proc(FILTER *fp, FILTER_PROC_INFO *fpip);
	BOOL func_update(FILTER *fp, int status);
	BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, void *editp, FILTER *fp);

	void getWaveform(FILTER *fp);
	void getAllWaveform(FILTER *fp);
	void deleteWaveform(FILTER *fp);
	void deleteAllWaveform(FILTER *fp);
	void drawWaveform(HDC dc, LPCRECT rc);
};

extern CShowWaveformApp theApp;

//---------------------------------------------------------------------
