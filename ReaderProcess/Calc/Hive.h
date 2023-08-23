#pragma once

namespace Calc
{
#pragma pack(1)
	//
	// ���̍\���̂� 24bit �����v�Z���s�����߂Ɏg�p����܂��B
	//
	struct int24_t {
		BYTE n[3];
		operator int32_t() const {
			struct {
				int32_t n:24;
			} s = { *(int32_t*)n };
			return s.n;
		}
	};
#pragma pack()

	//
	// ���̃N���X�͌v�Z�ɕK�v�ȕϐ���ێ����܂��B
	//
	inline struct Hive
	{
		//
		// �v�Z���ʂ��i�[���邽�߂̕ϐ��ł��B
		//
		ReaderBottle* bottle;

		//
		// �v�Z�ɕK�v�ȕϐ��ł��B
		//
		WAVEFORMATEX audio_format;

		//
		// �v�Z�����s����O�ɂ��̊֐����Ăяo����
		// �v�Z���ʂ��i�[����{�g�����Z�b�g���Ă��������B
		//
		void setBottle(ReaderBottle* bottle)
		{
			this->bottle = bottle;
		}

		//
		// �v�Z�����s����O�ɂ��̊֐����Ăяo����
		// �v�Z�ɕK�v�ȃI�[�f�B�I�t�H�[�}�b�g���Z�b�g���Ă��������B
		//
		void setAudioFormat(const WAVEFORMATEX& audio_format)
		{
			this->audio_format = audio_format;
		}
	} hive;
}
