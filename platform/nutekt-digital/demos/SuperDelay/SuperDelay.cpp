/*
 * File: biquad.cpp
 *
 * Simple execution environment test using provided biquad filters.
 *
 * 
 * 
 * 2018 (c) Korg
 *
 */

#include "userdelfx.h"
#include "SuperDelayModule.hpp"
#include "NTS.hpp"

#define SAMPLERATE 48000

#define DELAYBUFFERSIZE 5 * SAMPLERATE

ByteFarm::Dsp::SuperDelayModule<DELAYBUFFERSIZE, SAMPLERATE> _module; 

void DELFX_INIT(uint32_t platform, uint32_t api)
{
  (void)platform;
  (void)api;
}

void DELFX_PROCESS(float *xn, uint32_t frames)
{
  _module.ProcessInPlace(xn , frames);
}


void DELFX_PARAM(uint8_t index, int32_t value)
{
  _module.UpdateParams(index, value);
}

