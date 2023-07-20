#include "pch.h"
#include "App.h"

//--------------------------------------------------------------------

// キャッシュをボトルに詰めてからメインプロセスに送る。(受け取るように促す)
BOOL App::sendCache(const CachePtr& cache)
{
	MY_TRACE(_T("App::sendCache(%hs)\n"), cache->fileName.c_str());

	ReceiverBottle* shared = ::shared.receiverBottle.getBuffer();
	MY_TRACE_HEX(shared);
	if (!shared) return FALSE;

	::StringCbCopyA(shared->fileName, sizeof(shared->fileName), cache->fileName.c_str());

	shared->volumeCount = (int32_t)cache->volumes.size();
	MY_TRACE_INT(shared->volumeCount);
	memcpy(shared->volumes, cache->volumes.data(), sizeof(Volume) * cache->volumes.size());

	MY_TRACE_HEX(windowContainer);
	::SendMessage(windowContainer, WM_AVIUTL_FILTER_RECEIVE, 0, 0);

	return TRUE;
}

BOOL App::recalcTotals()
{
	MY_TRACE(_T("App::recalcTotals()\n"));

	// プロジェクトパラメータが有効かどうかチェックする。
	if (!projectParams) return FALSE;
	if (projectParams->frameNumber <= 0) return FALSE;

	// 全てのアイテムのキャッシュが作成済みかチェックする。
	for (auto pair : audioParamsMap)
	{
		const AudioParamsPtr& params = pair.second;
		if (params->sceneSet != projectParams->sceneIndex) continue;
		const CachePtr& cache = cacheManager.getCache(params->fileName);
		if (!cache) return FALSE;
	}

	// バッファを確保する。
	totals.resize(projectParams->frameNumber);
	memset(totals.data(), 0, sizeof(totals[0]) * totals.size());

	float scale = (float)Volume::Resolution * projectParams->video_scale / projectParams->video_rate;
	MY_TRACE_REAL(scale);

	for (auto pair : audioParamsMap)
	{
		const AudioParamsPtr& params = pair.second;
		if (params->sceneSet != projectParams->sceneIndex) continue;
		if (params->layerFlag & (uint32_t)ExEdit::LayerSetting::Flag::UnDisp) continue;
		const CachePtr& cache = cacheManager.getCache(params->fileName);
		if (!cache) continue;

		// 現在のフレームレートでの1フレーム毎の音量に変換する。

		int32_t frameBegin = params->frameBegin;
		int32_t frameEnd = std::min(params->frameEnd + 1, projectParams->frameNumber);
		int32_t frameRange = frameEnd - frameBegin;
		for (int32_t i = 0; i < frameRange; i++)
		{
			float temp1 = scale * params->playSpeed * i;
			float temp2 = scale * params->playBegin;
			int32_t src = (int32_t)(temp1 + temp2);
			int32_t dst = i + frameBegin;

			if (src < 0 || src >= (int32_t)cache->volumes.size())
				break;

			if (dst < 0 || dst >= (int32_t)totals.size())
				break;

			totals[dst].level += cache->volumes[src].level * params->volume;
		}
	}

	int c = (int)totals.size();
	MY_TRACE_INT(c);
	for (int i = 0; i < c; i++)
	{
		totals[i].rms = 20 * log10f(totals[i].level);

//		MY_TRACE(_T("%d : %f, %f\n"), i, totals[i].level, totals[i].rms);
	}

	mainWindow.recalcLayout();
	mainWindow.outputFrames();
	mainWindow.redraw();

	return TRUE;
}

BOOL App::setProjectParams(const ProjectParamsPtr& params)
{
	MY_TRACE(_T("App::setProjectParams()\n"));

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
			mainWindow.redraw();

			// カレントフレームをステータスボックスに出力する。
			mainWindow.outputFrames();
		}

		return FALSE;
	}

	projectParams = params;
	MY_TRACE_INT(projectParams->video_scale);
	MY_TRACE_INT(projectParams->video_rate);
	MY_TRACE_INT(projectParams->frameNumber);
	MY_TRACE_INT(projectParams->sceneIndex);
	MY_TRACE_INT(projectParams->currentFrame);

	if (::IsWindowVisible(mainWindow))
	{
		// 全体の音声波形を再計算する。
		recalcTotals();
	}

	return TRUE;
}

BOOL App::setAudioParams(const AudioParamsPtr& params)
{
	MY_TRACE(_T("App::setAudioParams()\n"));

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

	if (::IsWindowVisible(mainWindow))
	{
		// 全体の音声波形を再計算する。
		recalcTotals();
	}

	return TRUE;
}

BOOL App::setTotalsParams(const TotalsParamsPtr& params)
{
	MY_TRACE(_T("App::setTotalsParams()\n"));

	MY_TRACE_INT(params->showBPM);
	MY_TRACE_INT(params->tempo.orig);
	MY_TRACE_INT(params->tempo.bpm);
	MY_TRACE_INT(params->tempo.above);
	MY_TRACE_INT(params->tempo.below);

	setShowBPM(params->showBPM);
	setOrig(params->tempo.orig);
	setBPM(params->tempo.bpm);
	setAbove(params->tempo.above);
	setBelow(params->tempo.below);
	mainWindow.updateShared();

	if (::IsWindowVisible(mainWindow))
	{
		// 全体の音声波形を再描画する。
		mainWindow.redraw();
	}

	return TRUE;
}

//--------------------------------------------------------------------
