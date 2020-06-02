// Delay.h
#pragma once
#include "CircularBuffer.hpp"
#include "FxElement.hpp"
#include "TypedArray.hpp"
#include <inttypes.h>
#include "LRSample32F.hpp"

namespace ByteFarm
{
	namespace Dsp
	{

		class DelayParams
		{
		public:
			volatile float Mix = 0.5f;
			volatile float Feedback = 0.15f;
			volatile float TimeMs = 2500;
			DelayParams(float mix, float feedback, float timeMs)
			{
				Mix = mix;
				Feedback = feedback;
				TimeMs = timeMs;
			}
		};

		template <size_t BufferSize, uint16_t SampleRate, uint8_t NumDelayProcessors, uint8_t NumFeedbackProcessors>
		class DelayBase : public FxElement<DelayParams, SampleRate>
		{

			CircularBuffer<LRSample32F, BufferSize, uint32_t> _buffer;
			TypedArray<FxElementBase *, NumDelayProcessors, uint8_t> *_delayProcessors;
			TypedArray<FxElementBase *, NumFeedbackProcessors, uint8_t> *_feedbackProcessors;
			uint16_t FramesPerMs = SampleRate / 1000;

		public:
			DelayBase(DelayParams *params, TypedArray<FxElementBase *, NumDelayProcessors, uint8_t> *delayProcessors,
					  TypedArray<FxElementBase *, NumFeedbackProcessors, uint8_t> *feedbackProcessors) : FxElement<DelayParams, SampleRate>(params)
			{
				//_buffer = new CircularBuffer<LRSample32F, BufferSize, uint32_t>();
				_delayProcessors = delayProcessors;
				_feedbackProcessors = feedbackProcessors;
			}

			inline void Increment() override
			{
				_buffer.Increment();
				for (int i = 0; i < _delayProcessors->Size(); i++)
				{
					_delayProcessors->Get(i)->Increment();
				}
				for (int j = 0; j < _feedbackProcessors->Size(); j++)
				{
					_feedbackProcessors->Get(j)->Increment();
				}
			}

			inline LRSample32F Process(LRSample32F sample) override
			{
				LRSample32F s = sample.Clone();

				for (int i = 0; i < _delayProcessors->Size(); i++)
				{
					s = _delayProcessors->Get(i)->Process(s);
				}

				LRSample32F echo = _buffer.Read((int32_t)(this->Params->TimeMs * -FramesPerMs)) * (0.45f * this->Params->Feedback);
				for (int j = 0; j < _feedbackProcessors->Size(); j++)
				{
					echo = _feedbackProcessors->Get(j)->Process(echo);
				}
				
				//LRSample32F w = s + echo;
				LRSample32F w = s + (echo * this->Params->Mix);

				_buffer.Write(LRSample32F{
					fx_softclipf(0.05f, w.Left),
					fx_softclipf(0.05f, w.Right)
				});

				return  (sample * (1.f - this->Params->Mix)) + (w * this->Params->Mix);
			}

			inline void SetBufferBlockSize(uint32_t blockSize){
				this->_buffer.SetBlockSize(blockSize);
			}

			~DelayBase()
			{
				//delete _buffer;
				delete _delayProcessors;
				delete _feedbackProcessors;
			}
		};
	} // namespace Dsp
} // namespace ByteFarm