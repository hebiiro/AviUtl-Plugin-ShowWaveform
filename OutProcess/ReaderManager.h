#pragma once

//--------------------------------------------------------------------

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

//--------------------------------------------------------------------

struct ReaderManager {
	ReaderMap readerMap;
	int maxReaderCount = 1;

	ReaderPtr getReader(DWORD id);
	ReaderPtr createReader(LPCSTR fileName);
	void eraseReader(DWORD id);
	BOOL hasEmpty();
};

//--------------------------------------------------------------------
