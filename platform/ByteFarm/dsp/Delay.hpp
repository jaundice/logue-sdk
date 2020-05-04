// Delay.h
#pragma once
#include "CircularBuffer.hpp"
#include "FxElement.hpp"
#include <array>
#include <vector>

namespace ByteFarm
{
	namespace Dsp
	{

		class DelayParams
		{
		public:
			volatile float Mix;
			volatile float Feedback;
			volatile long Time;
		};

		template <std::size_t BufferSize>
		class DelayBase : public FxElement<DelayParams>
		{
			CircularBuffer<LRSample32F, BufferSize> _buffer;
			std::vector<IProcessor *> _delayProcessors;
			std::vector<IProcessor *> _feedbackProcessors;
			//DelayParams* _params;

			DelayBase(DelayParams *params, std::vector<IProcessor *> delayProcessors[],
					  std::vector<IProcessor *> feedbackProcessors[]) : FxElement<DelayParams>(params)
			{
				//_params = params;
				_buffer = new CircularBuffer<LRSample32F, BufferSize>();
				_delayProcessors = delayProcessors;
				_feedbackProcessors = feedbackProcessors;
			}

			void Increment() override
			{
				_buffer.Increment();
				for (int i = 0; i < _delayProcessors.size(); i++)
				{
					_delayProcessors[i]->Increment();
				}
				for (int j = 0; j < _feedbackProcessors.size(); j++)
				{
					_feedbackProcessors[j]->Increment();
				}
			}

			LRSample32F Process(LRSample32F sample) override
			{
				LRSample32F s = sample.Clone();

				for (int i = 0; i < _delayProcessors.size(); i++)
				{
					s = _delayProcessors[i]->Process(s);
				}

				LRSample32F echo = _buffer.Read(this->Params->Time);
				for (int j = 0; j < _feedbackProcessors.size(); j++)
				{
					echo = _feedbackProcessors[j]->Process(echo);
				}

				echo = echo * this->Params->Feedback;

				LRSample32F w = s + echo;
				_buffer.Write(w);

				return (sample * (1 - this->Params->Mix)) + (w * this->Params->Mix);
			}
		};
	} // namespace Dsp
} // namespace ByteFarm