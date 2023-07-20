#pragma once

namespace Calc
{
#pragma pack(1)
	//
	// この構造体は 24bit 整数計算を行うために使用されます。
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
	// このクラスは計算に必要な変数を保持します。
	//
	inline struct Hive
	{
		//
		// 計算結果を格納するための変数です。
		//
		ReaderBottle* bottle;

		//
		// 計算に必要な変数です。
		//
		WAVEFORMATEX audio_format;

		//
		// 計算を実行する前にこの関数を呼び出して
		// 計算結果を格納するボトルをセットしてください。
		//
		void setBottle(ReaderBottle* bottle)
		{
			this->bottle = bottle;
		}

		//
		// 計算を実行する前にこの関数を呼び出して
		// 計算に必要なオーディオフォーマットをセットしてください。
		//
		void setAudioFormat(const WAVEFORMATEX& audio_format)
		{
			this->audio_format = audio_format;
		}
	} hive;
}
