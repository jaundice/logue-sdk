// Delay.h
#pragma once
#include "CircularBuffer.hpp"
#include "FxElement.hpp"
#include "TypedArray.hpp"

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

		template <size_t BufferSize>
		class DelayBase : public FxElement<DelayParams>
		{
			CircularBuffer<LRSample32F, BufferSize> _buffer;
			TypedArray<IProcessor *, BufferSize> _delayProcessors;
			TypedArray<IProcessor *, BufferSize> _feedbackProcessors;
			//DelayParams* _params;

			DelayBase(DelayParams *params, TypedArray<IProcessor *> delayProcessors,
					  TypedArray<IProcessor *> feedbackProcessors) : FxElement<DelayParams>(params)
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

			~DelayBase()
			{
				delete _buffer;
				delete _delayProcessors;
				delete _feedbackProcessors;
			}
		};
	} // namespace Dsp
} // namespace ByteFarm