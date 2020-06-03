// modified from https://www.musicdsp.org/en/latest/Effects/70-alien-wah.html
#pragma once
#include <complex>
#include "LRSample32F.hpp"
#include "float_math.h"
#include "dsp/simplelfo.hpp"
#include "FxElement.hpp"
#include "common.h"

namespace ByteFarm
{
	namespace Dsp
	{
		template <size_t WAHBUFFERSIZE, size_t SAMPLERATE>
		class AlienWahParams
		{

		public:
			dsp::SimpleLFO lfo;
			volatile float fb;
			volatile int32_t delay;

			AlienWahParams(float frequency, float feedback, int16_t delayFrames)
			{
				lfo.setF0(frequency, 1.f / SAMPLERATE);
				this->fb = feedback;
				delay = fmin(delayFrames, WAHBUFFERSIZE / 4.f); //static_cast<int32_t>(d/ SAMPLERATE * SAMPLERATE);
				if (delay < 1)
					delay = 1;
			}
		};

		template <size_t WAHBUFFERSIZE, size_t SAMPLERATE>
		class AlienWah : public FxElement<AlienWahParams<WAHBUFFERSIZE, SAMPLERATE>,SAMPLERATE>
		{
			std::complex<float> delaybuf[WAHBUFFERSIZE];
			float sampleBufL[WAHBUFFERSIZE];
			float sampleBufR[WAHBUFFERSIZE];
			uint32_t t;
			std::complex<float> c;
			uint32_t k = 0;
			int32_t delayIdx = 0;

			float lfo;

			inline float ProcessSample(float s)
			{
				return s * (c + ((1.f - this->Params->fb) * s)).real() * 3;
			}

		public:
			AlienWah(AlienWahParams<WAHBUFFERSIZE, SAMPLERATE> *params)
				: FxElement<AlienWahParams<WAHBUFFERSIZE, SAMPLERATE>, SAMPLERATE>(params)
			{
				int16_t i;
				for (i = 0; i < WAHBUFFERSIZE; i++)
					delaybuf[i] = std::complex<float>(0, 0);
				t = 0;
			}

			inline virtual LRSample32F Process(LRSample32F sample) override
			{

				float l = ProcessSample(sample.Left);
				float r = ProcessSample(sample.Right);

				float invFB = 0.5f / this->Params->fb;

				sampleBufL[k] = l;
				sampleBufR[k] = r;

				int32_t sidx = (int32_t)((int32_t)k - (int32_t)(lfo * this->Params->delay)) % WAHBUFFERSIZE;

				return LRSample32F{
					invFB * l + invFB * sampleBufR[sidx] /* - sample.Left  - (invFB * lfo * mono) */,
					invFB * r + invFB * sampleBufL[sidx] /* - sample.Right - (invFB * lfo * mono) */};
			}

			inline virtual void Increment() override
			{
				++t;
				t %= SAMPLERATE;

				++k;
				k %= WAHBUFFERSIZE;

				this->Params->lfo.cycle();

				float lfo = this->Params->lfo.triangle_bi();

				c = std::complex<float>(fastercosf(lfo) * this->Params->fb, fastersinf(lfo) * this->Params->fb);

				delayIdx = (k - this->Params->delay /*- (int32_t)(lfo /3.f * this->Params->delay)*/) % (WAHBUFFERSIZE);

				const std::complex<float> c = (c * (-1.f * delaybuf[delayIdx]));
				delaybuf[k] = c;
			}
		};
	} // namespace Dsp
} // namespace ByteFarm
