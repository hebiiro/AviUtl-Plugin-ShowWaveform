#include "pch.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

BOOL MainWindow::initWaveform()
{
	MY_TRACE(_T("MainWindow::initWaveform()\n"));

	m_mutex.init(0, FALSE, FormatText(_T("ShowWaveform.Mutex.%08X"), g_parent));
	m_fileMapping.init(sizeof(Bottle), FormatText(_T("ShowWaveform.FileMapping.%08X"), g_parent));
	m_fileMappingProjectParams.init(sizeof(ProjectParams), FormatText(_T("ShowWaveform.FileMapping.ProjectParams.%08X"), g_parent));
	m_fileMappingAudioParams.init(sizeof(AudioParams), FormatText(_T("ShowWaveform.FileMapping.AudioParams.%08X"), g_parent));

	TCHAR fileName[MAX_PATH] = {};
	::GetModuleFileName(0, fileName, MAX_PATH);
	::PathRemoveFileSpec(fileName);
	::PathAppend(fileName, _T("..\\lwinput.aui"));
	MY_TRACE_TSTR(fileName);
	
	plugin = std::make_shared<Input::Plugin>();
	plugin->load(fileName);

	return TRUE;
}

BOOL MainWindow::termWaveform()
{
	MY_TRACE(_T("MainWindow::termWaveform()\n"));

	plugin->unload();
	plugin = 0;

	return TRUE;
}

//--------------------------------------------------------------------

BottlePtr MainWindow::getBottle()
{
	BottlePtr bottle = std::make_shared<Bottle>();

	{
//		Synchronizer sync(m_mutex);
		Bottle* shared = (Bottle*)m_fileMapping.getBuffer();

		if (!shared)
			return 0;

		memcpy(bottle.get(), shared, sizeof(*shared));
	}

	return bottle;
}

CachePtr MainWindow::getCache(const BottlePtr& bottle)
{
	auto it = cacheMap.find(bottle->fileName);
	if (it == cacheMap.end()) return 0;
	return it->second;
}

CachePtr MainWindow::createCache(const BottlePtr& bottle)
{
	MY_TRACE(_T("MainWindow::createCache()\n"));

	CachePtr cache = getCache(bottle);
	if (cache) return cache;

	cache = std::make_shared<Cache>();
	cacheMap[bottle->fileName] = cache;

	cache->fileName = bottle->fileName;
	MY_TRACE_STR(cache->fileName.c_str());

	getSample(cache);

	return cache;
}

void MainWindow::getSample(const CachePtr& cache)
{
	MY_TRACE(_T("MainWindow::getSample()\n"));

	Input::MediaPtr media = std::make_shared<Input::Media>();
	if (!media->open(plugin, cache->fileName.c_str()))
		return;

	AviUtl::InputPluginDLL* ip = plugin->getInputPlugin();
	AviUtl::InputHandle handle = media->getInputHandle();
	MediaInfo* mi = media->getMediaInfo();

	MY_TRACE_HEX(ip);
	MY_TRACE_HEX(handle);

	int32_t start = 0;
	int32_t length = mi->audio_format.nSamplesPerSec / SAMPLE_FPS;
	int32_t bufferSize = mi->audio_format.nBlockAlign * length;
	std::vector<BYTE> buffer(bufferSize);

	MY_TRACE_INT(mi->audio_format.wFormatTag);
	MY_TRACE_INT(mi->audio_format.nChannels);
	MY_TRACE_INT(mi->audio_format.nSamplesPerSec);
	MY_TRACE_INT(mi->audio_format.nAvgBytesPerSec);
	MY_TRACE_INT(mi->audio_format.nBlockAlign);
	MY_TRACE_INT(mi->audio_format.wBitsPerSample);
	MY_TRACE_INT(length);
	MY_TRACE_INT(bufferSize);

	for (int i = 0; ; i++)
	{
		int32_t read = ip->func_read_audio(handle, start, length, buffer.data());
		start += read;

		Sample sample;
		switch (mi->audio_format.wBitsPerSample)
		{
		case 8: sample.level = calc((const char*)buffer.data(), (int)(buffer.size() / sizeof(char))); break;
		case 16: sample.level = calc((const short*)buffer.data(), (int)(buffer.size() / sizeof(short))); break;
		case 24: sample.level = calc24((const BYTE*)buffer.data(), (int)(buffer.size() / 3)); break;
		case 32: sample.level = calc((const long*)buffer.data(), (int)(buffer.size() / sizeof(long))); break;
		}
		cache->samples.emplace_back(sample);

		MY_TRACE(_T("i = %d, read = %d, level = %f\n"), i, read, sample.level);

		if (read < length)
			break;
	}

	media->close();
}

void MainWindow::fireReceive(const CachePtr& cache)
{
	MY_TRACE(_T("MainWindow::fireReceive()\n"));

	{
//		Synchronizer sync(m_mutex);
		Bottle* shared = (Bottle*)m_fileMapping.getBuffer();
		MY_TRACE_HEX(shared);

		if (!shared)
			return;

		shared->sampleCount = (int32_t)cache->samples.size();
		MY_TRACE_INT(shared->sampleCount);
		memcpy(shared->samples, cache->samples.data(), sizeof(Sample) * cache->samples.size());
	}

	MY_TRACE_HEX(g_parent);
	::SendMessage(g_parent, WM_AVIUTL_FILTER_RECEIVE, 0, 0);
}

//--------------------------------------------------------------------

inline float MainWindow::normalize(char pcm)
{
	return pcm / 128.0f; // 8bit を -1.0 ～ 1.0 に正規化
}

inline float MainWindow::normalize(short pcm)
{
	return pcm / 32768.0f; // 16bit を -1.0 ～ 1.0 に正規化
}

inline float MainWindow::normalize(long pcm)
{
	return pcm / 2147483648.0f; // 32bit を -1.0 ～ 1.0 に正規化
}

inline float MainWindow::normalize(float pcm)
{
	return pcm;
}
#if 0
template<typename T>
float MainWindow::calc(const T* samples, int count)
{
	float level = 0.0f;
	for (int i = 0; i < count; i++)
	{
		T sample = samples[i];
		float n = normalize(sample);
		level = std::max(level, fabsf(n));
	}
	return level;
}
#else
template<typename T>
float MainWindow::calc(const T* samples, int count)
{
	if (!count) return 0.0f;

	float level = 0.0f;
	for (int i = 0; i < count; i++)
	{
		T sample = samples[i];
		float n = normalize(sample);
		level += n * n;
	}
	return sqrtf(level / (float)count);
}
#endif

inline float MainWindow::normalize24(long pcm)
{
	return pcm / 8388608.0f; // 24bit を -1.0 ～ 1.0 に正規化
}

inline long MainWindow::convert24(const BYTE* sample)
{
	long retValue = 0;
	retValue |= sample[0] << 8;
	retValue |= sample[1] << 16;
	retValue |= sample[2] << 24;
	return retValue >> 8;
}

float MainWindow::calc24(const BYTE* samples, int count)
{
	if (!count) return 0.0f;

	float level = 0.0f;
	for (int i = 0; i < count; i++)
	{
		long sample = convert24(samples + i * 3);
//		if (!(i % 100)) MY_TRACE_INT(sample);
		float n = normalize24(sample);
		level += n * n;
	}
	return sqrtf(level / (float)count);
}

//--------------------------------------------------------------------

void MainWindow::recalcWaveform()
{
	MY_TRACE(_T("MainWindow::recalcWaveform()\n"));

	// プロジェクトパラメータが有効かどうかチェックする。
	if (!projectParams) return;
	if (projectParams->frameNumber <= 0) return;

	// アイテムパラメータが有効かどうかチェックする。
	if (audioParamsMap.empty()) return;

	// 全てのアイテムのキャッシュが作成済みかチェックする。
	for (auto pair : audioParamsMap)
	{
		const AudioParamsPtr& params = pair.second;
		if (params->sceneSet != projectParams->sceneIndex) continue;
		auto it = cacheMap.find(params->fileName);
		if (it == cacheMap.end()) return;
	}

	// バッファを確保する。
	fullSamples.resize(projectParams->frameNumber);
	memset(fullSamples.data(), 0, sizeof(fullSamples[0]) * fullSamples.size());

	float scale = (float)SAMPLE_FPS * projectParams->video_scale / projectParams->video_rate;
	MY_TRACE_REAL(scale);

	for (auto pair : audioParamsMap)
	{
		const AudioParamsPtr& params = pair.second;
		if (params->sceneSet != projectParams->sceneIndex) continue;
		auto it = cacheMap.find(params->fileName);
		if (it == cacheMap.end()) continue;
		const CachePtr& cache = it->second;

		// 現在のフレームレートの1フレーム毎のサンプルに変換する。

		int32_t frameBegin = params->frameBegin;
		int32_t frameEnd = std::min(params->frameEnd + 1, projectParams->frameNumber);
		int32_t frameRange = frameEnd - frameBegin;
		for (int32_t i = 0; i < frameRange; i++)
		{
			float temp1 = scale * params->playSpeed * i;
			float temp2 = scale * params->playBegin;
			int32_t src = (int32_t)(temp1 + temp2);

			if (src >= (int32_t)cache->samples.size())
				break;

			fullSamples[i + frameBegin] += cache->samples[src].level * params->volume;
		}
	}

	int c = (int)fullSamples.size();
	MY_TRACE_INT(c);
	for (int i = 0; i < c; i++)
	{
		fullSamples[i] = 20 * log10f(fullSamples[i]);

		MY_TRACE(_T("%d : %f\n"), i, fullSamples[i]);
	}

	::InvalidateRect(m_hwnd, 0, FALSE);
}

//--------------------------------------------------------------------

ProjectParamsPtr MainWindow::getProjectParams()
{
	MY_TRACE(_T("MainWindow::getProjectParams()\n"));

	ProjectParamsPtr params = std::make_shared<ProjectParams>();

	{
//		Synchronizer sync(m_mutex);
		ProjectParams* shared = (ProjectParams*)m_fileMappingProjectParams.getBuffer();

		if (!shared)
			return 0;

		memcpy(params.get(), shared, sizeof(*shared));
	}

	return params;
}

void MainWindow::setProjectParams(const ProjectParamsPtr& params)
{
	MY_TRACE(_T("MainWindow::setProjectParams()\n"));

	if (projectParams &&
		projectParams->video_scale == params->video_scale &&
		projectParams->video_rate == params->video_rate &&
		projectParams->frameNumber == params->frameNumber &&
		projectParams->sceneIndex == params->sceneIndex)
	{
		MY_TRACE_INT(params->currentFrame);

		// カレントフレームが異なる場合は
		if (projectParams->currentFrame != params->currentFrame)
		{
			MY_TRACE(_T("カレントフレームが異なるので再描画します\n"));

			// カレントフレームを更新し、
			projectParams->currentFrame = params->currentFrame;

			// 再描画する。
			::InvalidateRect(m_hwnd, 0, FALSE);
		}

		return;
	}

	projectParams = params;
	MY_TRACE_INT(projectParams->video_scale);
	MY_TRACE_INT(projectParams->video_rate);
	MY_TRACE_INT(projectParams->frameNumber);
	MY_TRACE_INT(projectParams->sceneIndex);
	MY_TRACE_INT(projectParams->currentFrame);

	if (::IsWindowVisible(m_hwnd))
	{
		// 音声波形を再計算する。
		recalcWaveform();
	}
}

//--------------------------------------------------------------------

AudioParamsPtr MainWindow::getAudioParams()
{
	MY_TRACE(_T("MainWindow::getAudioParams()\n"));

	AudioParamsPtr params = std::make_shared<AudioParams>();

	{
//		Synchronizer sync(m_mutex);
		AudioParams* shared = (AudioParams*)m_fileMappingAudioParams.getBuffer();

		if (!shared)
			return 0;

		memcpy(params.get(), shared, sizeof(*shared));
	}

	return params;
}

void MainWindow::setAudioParams(const AudioParamsPtr& params)
{
	MY_TRACE(_T("MainWindow::setAudioParams()\n"));

	MY_TRACE_STR(params->fileName);
	MY_TRACE_HEX(params->id);
	MY_TRACE_HEX(params->flag);
	MY_TRACE_INT(params->frameBegin);
	MY_TRACE_INT(params->frameEnd);
	MY_TRACE_INT(params->sceneSet);
	MY_TRACE_REAL(params->volume);
	MY_TRACE_INT(params->playBegin);
	MY_TRACE_REAL(params->playSpeed);

	if (params->flag & (uint32_t)ExEdit::Object::Flag::Exist)
		audioParamsMap[params->id] = params;
	else
		audioParamsMap.erase(params->id);
}

//--------------------------------------------------------------------
