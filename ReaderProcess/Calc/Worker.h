#pragma once
#include "Hive.h"

namespace Calc
{
	using Buffer = std::vector<BYTE>;
	using BufferPtr = std::shared_ptr<Buffer>;

	//
	// ���̃N���X�͉����M�����特�ʂ��Z�o���܂��B
	//
	struct Worker
	{
		//
		// �v�Z�Ώۂ̃t���[���ԍ��ł��B
		//
		int i;

		//
		// �v�Z�Ώۂ̃t���[���̉����M���ł��B
		//
		BufferPtr buffer;

		static Worker* create(int i, const BufferPtr& buffer)
		{
			return new Worker(i, buffer);
		}

		static VOID CALLBACK WorkCallback(
			_Inout_     PTP_CALLBACK_INSTANCE Instance,
			_Inout_opt_ PVOID                 Context,
			_Inout_     PTP_WORK              Work)
		{
			Worker* worker = (Worker*)Context;
			worker->calcVolume();
			delete worker;
		}

		Worker(int i, const BufferPtr& buffer)
			: i(i)
			, buffer(buffer)
		{
		}

		void calcVolume()
		{
			Volume volume = {};
			switch (hive.audio_format.wBitsPerSample)
			{
			case  8: volume.level = calc((const  int8_t*)buffer->data(), (int)(buffer->size() / sizeof( int8_t))); break;
			case 16: volume.level = calc((const int16_t*)buffer->data(), (int)(buffer->size() / sizeof(int16_t))); break;
			case 24: volume.level = calc((const int24_t*)buffer->data(), (int)(buffer->size() / sizeof(int24_t))); break;
			case 32: volume.level = calc((const int32_t*)buffer->data(), (int)(buffer->size() / sizeof(int32_t))); break;
			default: volume.level = 0.0f; break;
			}
			hive.bottle->volumes[i] = volume;

			MY_TRACE(_T("i = %d, level = %f\n"), i, volume.level);
		}

		//
		// �󂯎���� 8bit �̉����M���� -1.0 �` 1.0 �ɐ��K�����ĕԂ��܂��B
		//
		static float normalize(int8_t pcm)
		{
			return pcm / 128.0f;
		}

		//
		// �󂯎���� 16bit �̉����M���� -1.0 �` 1.0 �ɐ��K�����ĕԂ��܂��B
		//
		static float normalize(int16_t pcm)
		{
			return pcm / 32768.0f;
		}

		//
		// �󂯎���� 24bit �̉����M���� -1.0 �` 1.0 �ɐ��K�����ĕԂ��܂��B
		//
		static float normalize(int24_t pcm)
		{
			return pcm / 8388608.0f;
		}

		//
		// �󂯎���� 32bit �̉����M���� -1.0 �` 1.0 �ɐ��K�����ĕԂ��܂��B
		//
		static float normalize(int32_t pcm)
		{
			return pcm / 2147483648.0f;
		}

		//
		// �󂯎���� 32bit ���������_���̉����M�������̂܂ܕԂ��܂��B
		//
		static float normalize(float pcm)
		{
			return pcm;
		}

		template<typename T>
		static float calc(const T* pcms, int count)
		{
			if (!count) return 0.0f;

			float level = 0.0f;
			for (int i = 0; i < count; i++)
			{
				T pcm = pcms[i];
				float n = normalize(pcm);
				level += n * n;
			}
			return sqrtf(level / (float)count);
		}
	};
}
