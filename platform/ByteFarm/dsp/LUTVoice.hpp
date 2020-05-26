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
        template <uint8_t NumOscillators, uint8_t NumEnvelopes, size_t LUTSize, size_t SampleRate>
        class LUTVoice : public Voice
        {
        protected:
            const float OneOverNumOscillators = 1.f / (float)NumOscillators;

        public:
            TypedArray<LUTOsc<LUTSize, SampleRate> *, NumOscillators, uint8_t> Oscillators;
            TypedArray<Envelope<SampleRate> *, NumEnvelopes, uint8_t> Envelopes;
            float Slop[NumEnvelopes]{0};

            virtual void IncrementEnvelope(uint32_t frames) override
            {
                for (uint8_t i = 0; i < NumEnvelopes; i++)
                {
                    Envelopes.Get(i)->Increment(frames);
                }
            };

            virtual void Reset() override
            {
                for (uint8_t i = 0; i < NumOscillators; i++)
                {
                    Oscillators.Get(i)->Reset();
                }
                for (uint8_t i = 0; i < NumEnvelopes; i++)
                {
                    Envelopes.Get(i)->Reset();
                }
            };

            inline virtual float OscillatorAmpModifier(uint8_t ocillatorIndex) 
            {
                return OneOverNumOscillators;
            }

            inline virtual float AmpEnvelopeValueForOscillator(uint8_t oscillatorNumber) 
            {
                return Envelopes.Get(oscillatorNumber % NumOscillators)->CurrentValue();
            }

            inline virtual float Generate() override
            {
                float ret = 0.f;
                for (uint8_t i = 0; i < NumOscillators; i++)
                {
                    ret += Oscillators.Get(i)->NextSample() * OscillatorAmpModifier(i) * AmpEnvelopeValueForOscillator(i);
                }
                return ret;
            };

            inline virtual void UpdateOscParams(VoiceParams params) override
            {
                Params = params;
                for (uint8_t i = 0; i < NumOscillators; i++)
                {
                    uint8_t octave = i % 3 *12u;
                    float freq = NoteToHz(params.NoteNumber -octave, params.Detune);
                    float lg = log2f(freq);
                    lg = i%2 == 0 ? lg : -1.f * lg;
                    Oscillators.Get(i)->SetFreq(freq + lg*osc_white());
                }
            };

            inline virtual void NoteOn() override
            {
                for (uint8_t i = 0; i < NumEnvelopes; i++)
                {
                    Envelopes.Get(i)->NoteOn();
                }
            };
            inline virtual void NoteOff() override
            {

                for (uint8_t i = 0; i < NumEnvelopes; i++)
                {
                    Envelopes.Get(i)->NoteOff();
                }
            };
        };
    } // namespace Dsp
} // namespace ByteFarm