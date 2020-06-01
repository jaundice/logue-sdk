#pragma once
#include "Voice.hpp"
#include <inttypes.h>
#include "LUTOsc.hpp"
#include "../tools/WaveTable/WaveTableGenerator.hpp"
#include "Envelope.hpp"
#include <assert.h>

namespace ByteFarm
{
    namespace Dsp
    {


        template <uint8_t NumOscillators, size_t LUTLength, size_t SampleRate>
        class FMAlgorithm
        {

        public:
            virtual float Generate() = 0;

            virtual void SetBaseFreq(float freq) = 0;

            inline virtual float ModulateFreq(float freq, float modValue)
            {
                return freq + modValue * freq;
            }
        };

        template <uint8_t NumOscillators, uint8_t NumAlgorithms,  size_t LUTLength, size_t SampleRate>
        class FMVoice : public Voice
        {
            float SineLut[LUTLength];
            FMAlgorithm<NumOscillators, LUTLength, SampleRate> * Algorithms[1];
            uint8_t AlgorithmInddex = 0;

        public:
            LUTOsc<LUTLength, SampleRate> *Oscillators[NumOscillators];
            Envelope<SampleRate> *Envelopes[NumOscillators + 1];
            float OscRatio[NumOscillators]{1.f};
            float OperatorLevel[NumOscillators]{1.f};

            FMVoice()
            {
                ByteFarm::Tools::WaveTableGenerator::Fill<LUTLength>(SineLut, ByteFarm::Tools::CommonWaveTableFunctors::Sine);
                for (uint8_t i = 0; i < NumOscillators; i++)
                {
                    Oscillators[i] = new LUTOsc<LUTLength, SampleRate>(
                        SineLut,
                        0.f);
                }

                for (uint8_t i = 0; i < NumOscillators + 1; i++)
                {
                    Envelopes[i] = new Envelope<SampleRate>(
                        Delay | Attack | Hold | Decay | Sustain | Release,
                        0.f,
                        5.f,
                        80.f,
                        300.f,
                        1000.f,
                        0.6f,
                        0.25f);
                }
            }

            void SetAlgorithm(uint8_t index, FMAlgorithm<NumOscillators, LUTLength, SampleRate> * algo){
                Algorithms[index] = algo;
            }

            void SetRate(uint8_t oscIndex, float rate)
            {
                OscRatio[oscIndex] = rate;
            }

            float Generate() override
            {
                for(uint8_t i=0;i<NumOscillators+1;i++){
                    this->Envelopes[i]->Increment();
                }
                return this->Algorithms[this->AlgorithmInddex]->Generate();
            }

            inline virtual void NoteOn() override
            {
                for (uint8_t i = 0; i < NumOscillators+1; i++)
                {
                    Envelopes[i]->NoteOn();
                }
            };

            inline virtual void NoteOff() override
            {
                for (uint8_t i = 0; i < NumOscillators+1; i++)
                {
                    Envelopes[i]->NoteOff();
                }
            };

            inline virtual void UpdateOscParams(VoiceParams params) override
            {
                Params = params;
                    //uint8_t octave = i % 3 * 12u;
                float freq = NoteToHz(params.NoteNumber, params.Detune);
                this->Algorithms[this->AlgorithmInddex]->SetBaseFreq(freq);
            };

            Envelope<SampleRate> *GetEnvelope(uint8_t envIdx)
            {
                return Envelopes[envIdx];
            }

            void IncrementEnvelope(uint32_t frames) override
            {
            }

            void Reset() override
            {
            }

            ~FMVoice()
            {
                for (uint8_t i = 0; i < NumOscillators; i++)
                {
                    delete Oscillators[i];
                }
                for (uint8_t i = 0; i < NumOscillators + 1; i++)
                {
                    delete Envelopes[i];
                }
            }
        };


        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo1 : public FMAlgorithm<6, LUTLength, SampleRate>
        {
            float OscBaseFrequency[6];

            float op6Feedback = 0.f;
            FMVoice<6, 1, LUTLength, SampleRate> *Voice;
            
        public:
            DX7Algo1(FMVoice<6, 1, LUTLength, SampleRate> *voice)
            {
                Voice = voice;
            }

            void SetBaseFreq(float freq) override
            {
                for (uint8_t i = 0; i < 6; i++)
                {
                    OscBaseFrequency[i] = freq * this->Voice->OscRatio[i];
                }
            }

            float Generate() override
            {
                this->Voice->Oscillators[1]->SetFreq(OscBaseFrequency[1]);
                float op2 = this->Voice->Oscillators[1]->NextSample() * this->Voice->Envelopes[1]->CurrentValue() * this->Voice->OperatorLevel[1];

                this->Voice->Oscillators[0]->SetFreq(this->ModulateFreq(OscBaseFrequency[0], op2));

                float op1 = this->Voice->Oscillators[0]->NextSample() * this->Voice->Envelopes[0]->CurrentValue() * this->Voice->OperatorLevel[0];

                this->Voice->Oscillators[5]->SetFreq(this->ModulateFreq(OscBaseFrequency[5], op6Feedback));

                float op6 = this->Voice->Oscillators[5]->NextSample() * this->Voice->Envelopes[5]->CurrentValue() * this->Voice->OperatorLevel[5];
                op6Feedback = op6;

                this->Voice->Oscillators[4]->SetFreq(this->ModulateFreq(OscBaseFrequency[4], op6));
                float op5 = this->Voice->Oscillators[4]->NextSample() * this->Voice->Envelopes[4]->CurrentValue() * this->Voice->OperatorLevel[4];

                this->Voice->Oscillators[3]->SetFreq(this->ModulateFreq(OscBaseFrequency[3], op5));
                float op4 = this->Voice->Oscillators[3]->NextSample() * this->Voice->Envelopes[3]->CurrentValue() * this->Voice->OperatorLevel[3];

                this->Voice->Oscillators[2]->SetFreq(this->ModulateFreq(OscBaseFrequency[2], op4));
                float op3 = this->Voice->Oscillators[2]->NextSample() * this->Voice->Envelopes[2]->CurrentValue() * this->Voice->OperatorLevel[2];

                return (op1 + op3) * this->Voice->Envelopes[6]->CurrentValue();
            }
        };

    } // namespace Dsp
} // namespace ByteFarm