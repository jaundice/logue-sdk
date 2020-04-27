// algo from https://www.musicdsp.org/en/latest/Effects/70-alien-wah.html

#include <complex>
#include "usermodfx.h"
#include "LRSample32F.h"
#include "float_math.h"

#define SAMPLERATE 48000
#define WAHBUFFERSIZE SAMPLERATE / 128

class AlienWahParams
{
public:
	float freq, startphase, fb;
	int16_t delay;

	AlienWahParams(float sphase, float f, int16_t d)
	{
		freq = f;
		startphase = sphase;
		fb = f / 4 + 0.74;
		delay = static_cast<int>(d/ 44100.0 * SAMPLERATE);
		if (delay < 1) delay = 1;
	}
};

class AlienWah 
{
	AlienWahParams* _params;
	std::complex<float> delaybuf[WAHBUFFERSIZE];
	float lfoskip;
	int32_t t;
	std::complex<float> c;
	int16_t k = 0;


public:

	inline AlienWah(AlienWahParams* params)
	{
		_params = params;


		;
		int16_t i;
		for (i = 0; i < WAHBUFFERSIZE; i++) 
			delaybuf[i] = std::complex<float>(0, 0);
		//lfoskip = _params->freq * 2 * 3.141592653589 / SAMPLERATE;
		t = 0;
	}

	inline void Increment() 
	{
		t++;
		const float lfo = (1 + fastcosf(t * (_params->freq * 2 * 3.141592653589 / SAMPLERATE) + _params->startphase));
		c = std::complex<float>(fastcosf(lfo) * _params->fb, fastsinf(lfo) * _params->fb);

		k++;
		k %= WAHBUFFERSIZE;
	}


	inline float ProcessSample(float s)
	{
		std::complex<float> outc = c * delaybuf[k] + (1 - _params->fb) * s;
		delaybuf[k] = outc;
		float out = outc.real() * 3; //take real part of outc
		//if (out < -32768) out = -32768;
		//else if (out > 32767) out = 32767; //Prevents clipping

		out = fx_softclipf(0.95, out);

		return out;
	}

	inline LRSample32F Process(LRSample32F sample)
	{
		return LRSample32F{
			ProcessSample(sample.Left),
			ProcessSample(sample.Right)
		};
	}
};