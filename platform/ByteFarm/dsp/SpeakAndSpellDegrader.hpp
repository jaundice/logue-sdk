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

		class SpeakAndSpellDegrader : public BitReducer
		{
		private:
			volatile uint8_t step = 0;

			LRSample32F _sample{0, 0};

		public:
			SpeakAndSpellDegrader(SpeakAndSpellDegraderParams *params) : BitReducer(static_cast<BitReducerParams *>(params))
			{
			}

			void Increment() override
			{
				step = (++step % this->GetSandSParams()->Downsample);
			}

			LRSample32F Process(LRSample32F sample) override
			{
				if (step > 0)
					return _sample;

				_sample = BitReducer::Process(sample);

				return _sample;
			}
			SpeakAndSpellDegraderParams *GetSandSParams()
			{
				return static_cast<SpeakAndSpellDegraderParams *>(this->Params);
			}
		};
	} // namespace Dsp
} // namespace ByteFarm