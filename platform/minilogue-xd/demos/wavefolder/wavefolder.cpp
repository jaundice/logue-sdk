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
float MaxLimit;
float MinLimit;
float Aggression;
float Gain;

// float powf_fast(float a, float b)
// {
//   union {
//     float d;
//     int x;
//   } u = {a};
//   u.x = (int)(b * (u.x - 1064866805) + 1064866805);
//   return u.d;
// }

float Delta(float delta)
{
  return fasterpowf(fabs(delta), Aggression);
}

inline float Folder(float s)
{
  return s > MaxLimit ? MaxLimit - Delta(s - MaxLimit) : MinLimit + Delta(MinLimit - s);
}

inline float Fold(float s)
{
  if (s > MinLimit && s < MaxLimit)
    return s;

  if (s < MinLimit || s > MaxLimit)
  {
    s = Folder(s);
  }
  if (s < MinLimit || s > MaxLimit)
  {
    s = Folder(s);
  }
  if (s < MinLimit || s > MaxLimit)
  {
    s = Folder(s);
  }
  if (s < MinLimit || s > MaxLimit)
  {
    s = Folder(s);
  }  
  if (s < MinLimit || s > MaxLimit)
  {
    s = Folder(s);
  }
  if (s < MinLimit || s > MaxLimit)
  {
    s = Folder(s);
  }
  if (s < MinLimit || s > MaxLimit)
  {
    s = fx_softclipf(0, s); //fmax(MinLimit, fmin(MaxLimit, s)); //we are bouncing high and low too much so we just clip
  }

  return s;
}

void MODFX_INIT(uint32_t platform, uint32_t api)
{
  (void)platform;
  (void)api;

  MaxLimit = 1.f;
  MinLimit = -1.f;
  Aggression = 1.f;
  Gain = 1.f;
}

float slewRate = 1.f;

void MODFX_PROCESS(const float *main_xn, float *main_yn, const float *sub_xn, float *sub_yn, uint32_t frames)
{
  const float *mx = main_xn;
  float *__restrict my = main_yn;
  const float *my_e = my + 2 * frames;

  const float *sx = sub_xn;
  float *__restrict sy = sub_yn;

  float invGain = 1.f / Gain;
  // Loop through the samples
  for (; my != my_e;)
  {
    // Pass sub through for prologue for now (L,R), you will need a separate filter at least if you want to process this properly
    *(sy++) = *(sx++); // Copy *sy to *sx (Left channel)
    *(sy++) = *(sx++); // Copy *sy to *sx (Right channel)

    float s = Gain * (*mx++);
    float s2 = Gain * (*mx++);

    float left = Fold(s);
    float right = Fold(s2);

    *(my++) = fx_softclipf(0.95f,  left);
    *(my++) = fx_softclipf(0.95f, right);
  }
}

void MODFX_PARAM(uint8_t index, int32_t value)
{
  float val = fabs(q31_to_f32(value));

  switch (index)
  {
  case 0:
  {
    val = 1.f - val;
    val = fmax(0.0001f, val);

    MaxLimit = val;
    MinLimit = -1.f * val;
    Gain = 1.f / val;
    break;
  }
  case 1:
  {
    Aggression = 0.5f + (10 * (1.f - val));
    break;
  }
  }
}
