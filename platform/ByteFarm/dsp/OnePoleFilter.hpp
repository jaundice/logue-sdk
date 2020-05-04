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

		class OnePoleLowPassFilter : public FxElement<OnePoleFilterParams>
		{
			LRSample32F _last{
				0, 0};

		public:
			OnePoleLowPassFilter(OnePoleFilterParams *params) : FxElement<OnePoleFilterParams>(params)
			{
			}

			void Increment() override
			{
			}

			LRSample32F Process(LRSample32F input) override
			{
				const LRSample32F delta{
					input.Left - _last.Left,
					input.Right - _last.Right};

				_last = _last + (delta * this->Params->Cutoff);

				return _last;
			}
		};
	} // namespace Dsp
} // namespace ByteFarm