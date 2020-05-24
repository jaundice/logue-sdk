#pragma once
#include "Voice.hpp"
#include <inttypes.h>
#include "TypedArray.hpp"
#include "LUTOsc.hpp"
#include "Envelope.hpp"

namespace ByteFarm
{
    namespace Dsp
    {
        template <uint8_t NumOcillators, size_t LUTSize, size_t SampleRate>
        class LUTVoice : public Voice
        {
        public:
            TypedArray<LUTOsc<LUTSize, SampleRate> *, NumOcillators, uint8_t> Oscillators;
            TypedArray<Envelope<SampleRate> *, NumOcillators, uint8_t> Envelopes;

            virtual void Increment() override
            {
                for (uint8_t i = 0; i < NumOcillators; i++)
                {
                    Envelopes.Get(i)->Increment();
                }
            };

            virtual void Reset() override
            {
                for (uint8_t i = 0; i < NumOcillators; i++)
                {
                    Oscillators.Get(i)->Reset();
                    Envelopes.Get(i)->Reset();
                }
            };
            virtual float Generate() override
            {
                float ret = 0.f;
                for (uint8_t i = 0; i < NumOcillators; i++)
                {
                    ret += Oscillators.Get(i)->NextSample() / NumOcillators /** Envelopes.Get(i)->CurrentValue()*/;
                }
                return ret;
            };

            virtual void UpdateOscParams(VoiceParams params) override
            {
                Params = params;
                Oscillators.Get(0)->SetFreq(NoteToHz(params.NoteNumber - 12u, params.Detune));
                Oscillators.Get(1)->SetFreq(NoteToHz(params.NoteNumber, params.Detune + (0.7f * osc_white())));
                Oscillators.Get(2)->SetFreq(NoteToHz(params.NoteNumber + 12u, params.Detune + (0.6f * osc_white())));
            };

            virtual void NoteOn() override
            {
                for (uint8_t i = 0; i < NumOcillators; i++)
                {
                    Envelopes.Get(i)->NoteOn();
                }
            };
            virtual void NoteOff() override
            {

                for (uint8_t i = 0; i < NumOcillators; i++)
                {
                    Envelopes.Get(i)->NoteOff();
                }
            };
        };
    } // namespace Dsp
} // namespace ByteFarm