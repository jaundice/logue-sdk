// OnePoleFilter.h
#pragma once
#include "FxElement.hpp"
#include "LRSample32F.hpp"

namespace ByteFarm
{
	namespace Dsp
	{

		class OnePoleFilterParams
		{
		public:
			volatile float Cutoff;
		};

		template<size_t SAMPLERATE>
		class OnePoleLowPassFilter : public FxElement<OnePoleFilterParams, SAMPLERATE>
		{
			LRSample32F _last{
				0, 0};

		public:
			OnePoleLowPassFilter(OnePoleFilterParams *params) : FxElement<OnePoleFilterParams, SAMPLERATE>(params)
			{
			}

			void Increment() override
			{
			}

			inline LRSample32F Process(LRSample32F input) override
			{
				const LRSample32F delta = input - _last;
				
				_last = _last + (delta * this->Params->Cutoff);

				return _last;
			}
		};
	} // namespace Dsp
} // namespace ByteFarm