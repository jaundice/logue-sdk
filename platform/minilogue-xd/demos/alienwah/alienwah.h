// modified from https://www.musicdsp.org/en/latest/Effects/70-alien-wah.html

#include <complex>
#include "usermodfx.h"
#include "LRSample32F.h"
#include "float_math.h"
#include "dsp/simplelfo.hpp"

#define SAMPLERATE 48000
#define WAHBUFFERSIZE 420

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

class AlienWah
{

	AlienWahParams *_params;
	std::complex<float> delaybuf[WAHBUFFERSIZE];
	uint32_t t;
	std::complex<float> c;
	uint32_t k = 0;

	float lfo;

	inline void Increment()
	{
		++t;
		t %= SAMPLERATE;

		++k;
		k %= WAHBUFFERSIZE;

		_params->lfo.cycle();

		float lfo = _params->lfo.triangle_bi();

		c = std::complex<float>(fastercosf(lfo) * _params->fb, fastersinf(lfo) * _params->fb);

		int32_t delayIdx = (k - _params->delay - (int32_t)(lfo * lfo * lfo * _params->delay)) % (WAHBUFFERSIZE);

		const std::complex<float> c = (c * (-1.f * delaybuf[delayIdx]));
		delaybuf[k] = c;
	}

	inline float ProcessSample(float s)
	{
		return s * (c + ((1.f - _params->fb) * s)).real() * 3;
	}

public:
	AlienWah(AlienWahParams *params)
	{
		_params = params;
		int16_t i;
		for (i = 0; i < WAHBUFFERSIZE; i++)
			delaybuf[i] = std::complex<float>(0, 0);
		t = 0;
	}

	inline LRSample32F Process(LRSample32F sample)
	{
		Increment();

		float l = ProcessSample(sample.Left);
		float r = ProcessSample(sample.Right);

		float mono = (sample.Left + sample.Right) / 2.f; //don't mix it out completely

		float invFB = 1.f / _params->fb;

		return LRSample32F{
			invFB * l -sample.Left /* - (invFB * lfo * mono) */,
			invFB * r -sample.Right /* - (invFB * lfo * mono) */};
	}
};