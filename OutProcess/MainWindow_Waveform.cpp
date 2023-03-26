#include "pch.h"
#include "MainWindow.h"

//--------------------------------------------------------------------

BOOL MainWindow::initWaveform()
{
	MY_TRACE(_T("MainWindow::initWaveform()\n"));

	m_sharedSenderBottle.init(getSharedSenderBottleName(g_parent));
	m_sharedReceiverBottle.init(getSharedReceiverBottleName(g_parent));
	m_sharedProjectParams.init(getSharedProjectParamsName(g_parent));
	m_sharedAudioParams.init(getSharedAudioParamsName(g_parent));

	return TRUE;
}

BOOL MainWindow::termWaveform()
{
	MY_TRACE(_T("MainWindow::termWaveform()\n"));

	return TRUE;
}

//--------------------------------------------------------------------

SenderBottlePtr MainWindow::getSenderBottle()
{
	SenderBottle* shared = (SenderBottle*)m_sharedSenderBottle.getBuffer();
	if (!shared) return 0;
	SenderBottlePtr bottle = std::make_shared<SenderBottle>();
	memcpy(bottle.get(), shared, sizeof(*shared));
	return bottle;
}

CachePtr MainWindow::getCache(LPCSTR fileName)
{
	MY_TRACE(_T("MainWindow::getCache(%hs)\n"), fileName);

	auto it = cacheMap.find(fileName);
	if (it == cacheMap.end()) return 0;
	return it->second;
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
		ProjectParams* shared = (ProjectParams*)m_sharedProjectParams.getBuffer();

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
		AudioParams* shared = (AudioParams*)m_sharedAudioParams.getBuffer();

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
