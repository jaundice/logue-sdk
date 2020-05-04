// BitReducer.h
#pragma once
#include "FxElement.hpp"
#include "LRSample32F.hpp"

namespace ByteFarm
{
	namespace Dsp
	{

		class BitReducerParams
		{
		public:
			volatile float Bits = 0;
			volatile float Quantum;

			void SetBits(float bits)
			{
				Bits = bits;
				Quantum = powf(2.0f, bits);
			}

			BitReducerParams()
			{
				SetBits(24);
			}
		};

		class BitReducer : public FxElement<BitReducerParams>
		{
			//BitReducerParams* _params;

		public:
			BitReducer(BitReducerParams *params) : FxElement<BitReducerParams>(params)
			{
				//_params = params;
			}

			void Increment() override
			{
			}

			LRSample32F Process(LRSample32F sample) override
			{
				// per sample
				return LRSample32F{
					floorf(sample.Left * this->Params->Quantum) / this->Params->Quantum,
					floorf(sample.Right * this->Params->Quantum) / this->Params->Quantum,
				};
			}
		};
	} // namespace Dsp
} // namespace ByteFarm