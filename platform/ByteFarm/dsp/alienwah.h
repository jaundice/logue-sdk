// modified from https://www.musicdsp.org/en/latest/Effects/70-alien-wah.html
#pragma once
#include <complex>
#include "usermodfx.h"
#include "LRSample32F.h"
#include "float_math.h"
#include "dsp/simplelfo.hpp"
#include "FxElement.h"
#include "common.h"

namespace ByteFarm
{
namespace Dsp
{
template <size_t WAHBUFFERSIZE>
class AlienWahParams
{

public:
	dsp::SimpleLFO lfo;
	volatile float fb;
	volatile int32_t delay;

	AlienWahParams(float f, float fb, int16_t d)
	{
		lfo.setF0(f, 1.f / SAMPLERATE);
		fb = 0.5f;
		delay = fmin(d, WAHBUFFERSIZE / 4.f); //static_cast<int32_t>(d/ SAMPLERATE * SAMPLERATE);
		if (delay < 1)
			delay = 1;
	}
};

template <size_t WAHBUFFERSIZE>
class AlienWah : public FxElement<AlienWahParams<WAHBUFFERSIZE>>
{
	std::complex<float> delaybuf[WAHBUFFERSIZE];
	uint32_t t;
	std::complex<float> c;
	uint32_t k = 0;

	float lfo;

	float ProcessSample(float s)
	{
		return s * (c + ((1.f - this->Params->fb) * s)).real() * 3;
	}

public:
	AlienWah(AlienWahParams<WAHBUFFERSIZE> *params)
		: FxElement<AlienWahParams<WAHBUFFERSIZE>>(params)
	{
		int16_t i;
		for (i = 0; i < WAHBUFFERSIZE; i++)
			delaybuf[i] = std::complex<float>(0, 0);
		t = 0;
	}

	virtual LRSample32F Process(LRSample32F sample) override
	{
		Increment();

		float l = ProcessSample(sample.Left);
		float r = ProcessSample(sample.Right);

		float mono = (sample.Left + sample.Right) / 2.f; //don't mix it out completely

		float invFB = 1.f / this->Params->fb;

		return LRSample32F{
			invFB * l - sample.Left /* - (invFB * lfo * mono) */,
			invFB * r - sample.Right /* - (invFB * lfo * mono) */};
	}

	virtual void Increment() override
	{
		++t;
		t %= SAMPLERATE;

		++k;
		k %= WAHBUFFERSIZE;

		this->Params->lfo.cycle();

		float lfo = this->Params->lfo.triangle_bi();

		c = std::complex<float>(fastercosf(lfo) * this->Params->fb, fastersinf(lfo) * this->Params->fb);

		int32_t delayIdx = (k - this->Params->delay - (int32_t)(lfo * lfo * lfo * this->Params->delay)) % (WAHBUFFERSIZE);

		const std::complex<float> c = (c * (-1.f * delaybuf[delayIdx]));
		delaybuf[k] = c;
	}

	virtual void UpdateParams(uint8_t paramIndex, int32_t value)
	{
		float val = fabs(q31_to_f32(value));

		switch (paramIndex)
		{
		case 0:
		{
			this->Params->lfo.setF0((fastexpf(val * val) - 1.f) * 8000.f, 1.f / SAMPLERATE);
			break;
		}
		case 1:
		{
			this->Params->delay = (int32_t)fmaxf(1, (val)*WAHBUFFERSIZE / 4.f) + 20 /* + WAHBUFFERSIZE/4.f */;
			this->Params->fb = 1.f + (fastexpf(val * val) - 1.f) * 30.f;
			break;
		}
		case 2:

			break;
		}
	}
};
} // namespace Dsp
} // namespace ByteFarm
