/*
    BSD 3-Clause License

    Copyright (c) 2018, KORG INC.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//*/

/*
 * File: waves.cpp
 *
 * Morphing wavetable oscillator
 *
 */

#include "usermodfx.h"
#include "wavefolder.hpp"

float Delta(float aggression, float delta) 
{
	return powf(delta, aggression);
}

float UnderMin(float min_limit, float aggression, float sample) 
{
	float s = sample;

	const float d = min_limit - s;

	s = min_limit + Delta(aggression, d);

	return s;
}


float OverMax(float max_limit, float aggression, float sample)
{
	float s = sample;

	const float d = s - max_limit;

	s = max_limit - Delta(aggression, d);

	return s;
}

  float MaxLimit;
	float MinLimit;
	float Aggression;
  float Gain;

  





	float Fold(float maxLimit, float minLimit, float aggression, float sample)
	{
		if (sample >= minLimit && sample <= maxLimit)
		{
			return sample;
		}

		float s = sample;

		do
		{
			if (s > maxLimit)
			{
				s = OverMax(maxLimit, aggression, s);
			}
			else
			{
				s = UnderMin(minLimit, aggression, s);
			}
		}
		while (s > maxLimit || s < minLimit);


		return s;
	}

	float Process(float sample) 
	{
		return Gain *	Fold(MaxLimit, MinLimit, Aggression, sample);
	}

void MODFX_INIT(uint32_t platform, uint32_t api){
  (void)platform;
  (void)api;

    MaxLimit =1.f;
    MinLimit =-1.f;
    Aggression =1.f;
    Gain = 1.f;
}

void MODFX_PROCESS(const float *main_xn, float *main_yn, const float *sub_xn, float *sub_yn, uint32_t frames){
   q31_t * __restrict y = (q31_t *)main_yn;
  const q31_t * y_e = y + frames;
  
  for (; y != y_e; ) {
    *(++main_yn) = Process(*(++main_xn));
  }
}

void MODFX_PARAM(uint8_t index, int32_t value)
{
    float val = q31_to_f32(value);

    switch(index){
      case 1:{
        MaxLimit = val;
        MinLimit = -1 * val; 
        Gain = 1.f / val;
        break;

      }
      case 2:{
        Aggression =val;
        break;
      }
    }

}


