// SpeakAndSpellDegrader.h
#pragma once
#include "BitReducer.hpp"
#include "LRSample32F.hpp"

namespace ByteFarm
{
	namespace Dsp
	{

		class SpeakAndSpellDegraderParams : public BitReducerParams
		{
		public:
			volatile uint8_t Downsample = 16;

			SpeakAndSpellDegraderParams() : BitReducerParams()
			{
			}
		};
		template <size_t SAMPLERATE>
		class SpeakAndSpellDegrader : public BitReducer<SAMPLERATE>
		{
		private:
			volatile uint8_t step = 0;

			LRSample32F _sample{0, 0};

		public:
			SpeakAndSpellDegrader(SpeakAndSpellDegraderParams *params) : BitReducer<SAMPLERATE>(static_cast<BitReducerParams *>(params))
			{
			}

			inline void Increment() override
			{
				step = (++step % this->GetSandSParams()->Downsample);
			}

			inline LRSample32F Process(LRSample32F sample) override
			{
				if (step > 0)
					return _sample;

				_sample = BitReducer<SAMPLERATE>::Process(sample);

				return _sample;
			}
			inline SpeakAndSpellDegraderParams *GetSandSParams()
			{
				return static_cast<SpeakAndSpellDegraderParams *>(this->Params);
			}
		};
	} // namespace Dsp
} // namespace ByteFarm