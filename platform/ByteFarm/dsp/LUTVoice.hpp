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

        template <uint8_t NumOscillators>
        class OscillatorTuningModifier
        {
        public:
            virtual int16_t GetOscillatorNote(uint8_t oscillatorIndex, int16_t note) = 0;
            virtual void SetSemis(int16_t semis) = 0;
        };

        template <uint8_t NumOscillators>
        class UnisonTuningModifier : public OscillatorTuningModifier<NumOscillators>
        {

            int16_t SemiTones;

        public:
            UnisonTuningModifier(int16_t semiTones)
            {
                SemiTones = semiTones;
            }

            void SetSemis(int16_t semis) override
            {
                SemiTones = semis;
            }

            int16_t GetOscillatorNote(uint8_t oscillatorIndex, int16_t note) override
            {

                note += SemiTones;

                if (note < 0)
                {
                    do
                    {
                        note += 12;
                    } while (note < 0);
                }

                return (int16_t)note;
            }
        };

        template <uint8_t NumOscillators, uint8_t NumOctaves>
        class OctavesTuningModifier : public OscillatorTuningModifier<NumOscillators>
        {
            int16_t Modifier[NumOscillators];
            int16_t SemiTones;

        public:
            void SetSemis(int16_t semis) override
            {
                SemiTones = semis;
            }

            OctavesTuningModifier(int16_t semiTones)
            {
                SemiTones = semiTones;

                for (uint8_t i = 0; i < NumOscillators; i++)
                {
                    Modifier[i] = (i % NumOctaves) * -12;
                }
            }

            int16_t GetOscillatorNote(uint8_t oscillatorIndex, int16_t note) override
            {
                note += Modifier[oscillatorIndex];
                note += SemiTones;

                if (note < 0)
                {

                    do
                    {
                        note += 12;
                    } while (note < 0);
                }

                return (int16_t)note;
            }
        };

        template <uint8_t NumOscillators>
        class OscillatorAmplitudeCalulator
        {
        public:
            virtual float GetOscillatorAmplitude(uint8_t oscillatorIndex, float modulator = 0.f) = 0;
            virtual float GetOverallAmplitude() = 0;
        };

        template <uint8_t NumOscillators>
        class PhasedAmplitudeCalculator : public OscillatorAmplitudeCalulator<NumOscillators>
        {
            float Sin[100];
            float Offset = 100.f / NumOscillators;

        public:
            PhasedAmplitudeCalculator()
            {
                float wt = 2 * PI / 100;
                for (int16_t i = 0; i < i; i++)
                {
                    Sin[i] = fastersinf(i * wt);
                }
            }

            float GetOscillatorAmplitude(uint8_t oscillatorIndex, float modulator = 0.f) override
            {
                return Sin[((int16_t)(modulator * 100.f) + (int16_t)((float)oscillatorIndex * Offset)) % 100] * 0.5f + 0.5f;
            }

            float GetOverallAmplitude() override
            {
                return 1.f;
            }
        };

        template <uint8_t NumOscillators>
        class EqualAmplitudeCalculator : public OscillatorAmplitudeCalulator<NumOscillators>
        {
            float OverallAmp = 1.f / (float)NumOscillators;

        public:
            float GetOscillatorAmplitude(uint8_t oscillatorIndex, float modulator = 0.f) override
            {
                return 1.f;
            }

            float GetOverallAmplitude() override
            {
                return OverallAmp;
            }
        };

        template <uint8_t NumOscillators, uint8_t NumEnvelopes, size_t LUTSize, size_t SampleRate>
        class LUTVoice : public Voice
        {
        protected:
            const float OneOverNumOscillators = 1.f / (float)NumOscillators;
            float Slop = 0.f;

        public:
            TypedArray<LUTOsc<LUTSize, SampleRate> *, NumOscillators, uint8_t> Oscillators;
            TypedArray<Envelope<SampleRate> *, NumEnvelopes, uint8_t> Envelopes;

            OscillatorAmplitudeCalulator<NumOscillators> *AmplitudeCalculator;
            OscillatorTuningModifier<NumOscillators> *TuningModifier;

            float AmpModulator = 0.5f;

            LUTVoice(float slop, OscillatorAmplitudeCalulator<NumOscillators> *ampCalculator, OscillatorTuningModifier<NumOscillators> *tuningModifier)
            {
                Slop = slop;
                AmplitudeCalculator = ampCalculator;
                TuningModifier = tuningModifier;
            }

            void SetSlop(float slop)
            {
                Slop = slop;
            }

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

            inline virtual float AmpEnvelopeValueForOscillator(uint8_t oscillatorNumber)
            {
                return Envelopes.Get(oscillatorNumber % NumEnvelopes)->CurrentValue();
            }

            inline virtual float Generate() override
            {
                float ret = 0.f;
                for (uint8_t i = 0; i < NumOscillators; i++)
                {
                    float ampMod = AmpModulator + Params.ShapeLfo;
                    if (ampMod < 0)
                        ampMod += 1.f;
                    ret += Oscillators.Get(i)->NextSample() * AmplitudeCalculator->GetOscillatorAmplitude(i, ampMod) * AmpEnvelopeValueForOscillator(i);
                }
                return ret * AmplitudeCalculator->GetOverallAmplitude();
            };

            inline virtual void UpdateOscParams(VoiceParams params) override
            {
                Params = params;
                for (uint8_t i = 0; i < NumOscillators; i++)
                {
                    //uint8_t octave = i % 3 * 12u;
                    float freq = NoteToHz(TuningModifier->GetOscillatorNote(i, params.NoteNumber), params.Detune);
                    float lg = log2f(freq);

                    Oscillators.Get(i)->SetFreq(freq + (lg * osc_white() * this->Slop));
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