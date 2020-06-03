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

        typedef float (*Modulator)(float freq, float modValue);

        inline float ModulateFreqAndAdd(float freq, float modValue)
        {
            return freq + modValue * freq;
        }
        inline float ModulateFreq(float freq, float modValue)
        {
            return modValue * freq;
        }

        template <uint8_t NumOscillators, size_t LUTLength, size_t SampleRate>
        class FMAlgorithm
        {

        public:
            virtual float Generate() = 0;

            virtual void SetBaseFreq(float freq) = 0;

            virtual void NormalizeVolume() = 0;

            virtual void Reset(float initFeedback) = 0;
        };

        template <uint8_t NumOscillators, size_t LUTLength, size_t SampleRate>
        class FMVoice : public Voice
        {
            float SineLut[LUTLength];
            FMAlgorithm<NumOscillators, LUTLength, SampleRate> *Algorithm;
            uint8_t AlgorithmIndex = 0;

        public:
            LUTOsc<LUTLength, SampleRate> *Oscillators[NumOscillators];
            Envelope<SampleRate> *Envelopes[NumOscillators + 1];
            float OscRatio[NumOscillators]{1.f};
            float OperatorLevel[NumOscillators]{1.f};

            int8_t PitchOffset = 0;
            Modulator Modulate = ModulateFreqAndAdd;

            bool ResetOscillatorsOnNoteOn = false;

            inline void SetModulator(Modulator mod)
            {
                this->Modulate = mod;
                // if (ResetOscillatorsOnNoteOn)
                //     this->ResetOscillators();
            }

            inline void ResetOscillators()
            {
                for (uint8_t i = 0; i < NumOscillators; i++)
                {
                    this->Oscillators[i]->Reset();
                }
                if (this->Algorithm != nullptr)
                    this->Algorithm->Reset((Modulator)(this->Algorithm) == (Modulator)ModulateFreqAndAdd ? 0.f : 1.f);
            }

            inline uint8_t GetAlgorithmIndex()
            {
                return this->AlgorithmIndex;
            }

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

            inline void SetAlgorithm(uint8_t index, FMAlgorithm<NumOscillators, LUTLength, SampleRate> *algo)
            {
                if (this->Algorithm != nullptr)
                    delete this->Algorithm;

                this->Algorithm = algo;
                this->AlgorithmIndex = index;
                // if (ResetOscillatorsOnNoteOn)
                //     this->ResetOscillators();
            }

            inline void SetRate(uint8_t oscIndex, float rate)
            {
                this->OscRatio[oscIndex] = rate;
            }

            inline float Generate() override
            {
                for (uint8_t i = 0; i < NumOscillators + 1; i++)
                {
                    this->Envelopes[i]->Increment();
                }
                return this->Algorithm->Generate();
            }

            inline virtual void NoteOn() override
            {
                if (ResetOscillatorsOnNoteOn)
                {
                    this->ResetOscillators();
                }

                for (uint8_t i = 0; i < NumOscillators + 1; i++)
                {
                    this->Envelopes[i]->NoteOn();
                }
            };

            inline virtual void NoteOff() override
            {
                for (uint8_t i = 0; i < NumOscillators + 1; i++)
                {
                    this->Envelopes[i]->NoteOff();
                }
            };

            inline virtual void UpdateOscParams(VoiceParams params) override
            {
                Params = params;
                int16_t n = params.NoteNumber + PitchOffset;
                while (n < 0)
                {
                    n += 12;
                }

                while (n > 126)
                {
                    n -= 12;
                }
                float freq = NoteToHz(n, params.Detune);
                this->Algorithm->SetBaseFreq(freq);
            };

            inline Envelope<SampleRate> *GetEnvelope(uint8_t envIdx)
            {
                return Envelopes[envIdx];
            }

            inline void IncrementEnvelope(uint32_t frames) override
            {
            }

            inline void Reset() override
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

                delete Algorithm;
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo : public FMAlgorithm<6, LUTLength, SampleRate>
        {

        protected:
            float OscBaseFrequency[6];
            FMVoice<6, LUTLength, SampleRate> *Voice;
            float Normalize = 1.f;

            inline float GetOscillatorValue(uint8_t opIndex)
            {
                return this->Voice->Oscillators[opIndex]->NextSample() * this->Voice->Envelopes[opIndex]->CurrentValue() * this->Voice->OperatorLevel[opIndex];
            }

            inline void SetOscillatorFrequency(uint8_t oscIndex, float modValue)
            {
                this->Voice->Oscillators[oscIndex]->SetFreq(this->Voice->Modulate(OscBaseFrequency[oscIndex], modValue));
            }

        public:
            DX7Algo(FMVoice<6, LUTLength, SampleRate> *voice)
            {
                Voice = voice;
            }

            inline void SetBaseFreq(float freq) override
            {
                for (uint8_t i = 0; i < 6; i++)
                {
                    this->OscBaseFrequency[i] = freq * this->Voice->OscRatio[i];
                }
            }

            float Generate() = 0;

            void NormalizeVolume() = 0;
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo1 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo1(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {
                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op5);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4);
                float op3 = this->GetOscillatorValue(2);

                return (op1 + op3) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo2 : public DX7Algo<LUTLength, SampleRate>
        {

            float op2Feedback = 0.f;

        public:
            DX7Algo2(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op2Feedback = initFeedback;
            }

            inline float Generate() override
            {
                this->SetOscillatorFrequency(1, op2Feedback);
                float op2 = this->GetOscillatorValue(1);
                op2Feedback = op2;

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, 0.f);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op5);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4);
                float op3 = this->GetOscillatorValue(2);

                return (op1 + op3) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo3 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo3(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(2, 0.f);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(1, op3);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op5);
                float op4 = this->GetOscillatorValue(3);

                return (op1 + op4) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[3]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo4 : public DX7Algo<LUTLength, SampleRate>
        {

            float op4Feedback = 0.f;

        public:
            DX7Algo4(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op4Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(2, 0.f);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(1, op3);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, op4Feedback);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op5);
                float op4 = this->GetOscillatorValue(3);
                op4Feedback = op4;

                return (op1 + op4) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[3]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo5 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo5(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {
                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(3, 0.f);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                return (op1 + op3 + op5) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[4]) + (this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo6 : public DX7Algo<LUTLength, SampleRate>
        {

            float op5Feedback = 0.f;

        public:
            DX7Algo6(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op5Feedback = initFeedback;
            }
            inline float Generate() override
            {
                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(3, 0.f);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(5, op5Feedback);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);
                op5Feedback = op5;

                return (op1 + op3 + op5) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[4]) + (this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo7 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo7(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {
                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, 0.f);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4 + op5);
                float op3 = this->GetOscillatorValue(2);

                return (op1 + op3) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo8 : public DX7Algo<LUTLength, SampleRate>
        {

            float op4Feedback = 0.f;

        public:
            DX7Algo8(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op4Feedback = initFeedback;
            }
            inline float Generate() override
            {
                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, 0.f);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op4Feedback);
                float op4 = this->GetOscillatorValue(3);
                op4Feedback = op4;

                this->SetOscillatorFrequency(2, op4 + op5);
                float op3 = this->GetOscillatorValue(2);

                return (op1 + op3) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo9 : public DX7Algo<LUTLength, SampleRate>
        {

            float op2Feedback = 0.f;

        public:
            DX7Algo9(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op2Feedback = initFeedback;
            }
            inline float Generate() override
            {
                this->SetOscillatorFrequency(1, op2Feedback);
                float op2 = this->GetOscillatorValue(1);
                op2Feedback = op2;

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, 0.f);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, 0.f);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4 + op5);
                float op3 = this->GetOscillatorValue(2);

                return (op1 + op3) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo10 : public DX7Algo<LUTLength, SampleRate>
        {

            float op3Feedback = 0.f;

        public:
            DX7Algo10(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op3Feedback = initFeedback;
            }
            inline float Generate() override
            {

                this->SetOscillatorFrequency(2, op3Feedback);
                float op3 = this->GetOscillatorValue(2);
                op3Feedback = op3;

                this->SetOscillatorFrequency(1, op3);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, 0.f);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, 0.f);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op6 + op5);
                float op4 = this->GetOscillatorValue(3);

                return (op1 + op4) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[3]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo11 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo11(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(2, 0.f);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(1, op3);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, 0.f);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op6 + op5);
                float op4 = this->GetOscillatorValue(3);

                return (op1 + op4) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[3]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo12 : public DX7Algo<LUTLength, SampleRate>
        {

            float op2Feedback = 0.f;

        public:
            DX7Algo12(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op2Feedback = initFeedback;
            }
            inline float Generate() override
            {

                this->SetOscillatorFrequency(1, op2Feedback);
                float op2 = this->GetOscillatorValue(1);
                op2Feedback = op2;

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(3, 0.f);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(5, 0.f);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, 0.f);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(2, op4 + op5 + op6);
                float op3 = this->GetOscillatorValue(2);

                return (op1 + op3) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo13 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo13(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(3, 0.f);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, 0.f);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(2, op4 + op5 + op6);
                float op3 = this->GetOscillatorValue(2);

                return (op1 + op3) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo14 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo14(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, 0.f);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op6 + op5);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4);
                float op3 = this->GetOscillatorValue(2);

                return (op1 + op3) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo15 : public DX7Algo<LUTLength, SampleRate>
        {

            float op2Feedback = 0.f;

        public:
            DX7Algo15(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op2Feedback = initFeedback;
            }
            inline float Generate() override
            {

                this->SetOscillatorFrequency(1, op2Feedback);
                float op2 = this->GetOscillatorValue(1);
                op2Feedback = op2;

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, 0.f);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, 0.f);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op6 + op5);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4);
                float op3 = this->GetOscillatorValue(2);

                return (op1 + op3) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo16 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo16(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(3, 0.f);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(0, op2 + op3 + op5);
                float op1 = this->GetOscillatorValue(0);

                return (op1) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo17 : public DX7Algo<LUTLength, SampleRate>
        {

            float op2Feedback = 0.f;

        public:
            DX7Algo17(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op2Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(1, op2Feedback);
                float op2 = this->GetOscillatorValue(1);
                op2Feedback = op2;

                this->SetOscillatorFrequency(3, 0.f);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(5, 0.f);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(0, op2 + op3 + op5);
                float op1 = this->GetOscillatorValue(0);

                return (op1) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo18 : public DX7Algo<LUTLength, SampleRate>
        {

            float op3Feedback = 0.f;

        public:
            DX7Algo18(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op3Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(2, op3Feedback);
                float op3 = this->GetOscillatorValue(2);
                op3Feedback = op3;

                this->SetOscillatorFrequency(5, 0.f);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op5);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(0, op2 + op3 + op5);
                float op1 = this->GetOscillatorValue(0);

                return (op1) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo19 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo19(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(2, 0.f);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op6);
                float op4 = this->GetOscillatorValue(3);

                return (op1 + op4 + op5) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[3]) + (this->Voice->OperatorLevel[4]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo20 : public DX7Algo<LUTLength, SampleRate>
        {

            float op3Feedback = 0.f;

        public:
            DX7Algo20(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op3Feedback = initFeedback;
            }
            inline float Generate() override
            {

                this->SetOscillatorFrequency(2, op3Feedback);
                float op3 = this->GetOscillatorValue(2);
                op3Feedback = op3;

                this->SetOscillatorFrequency(1, op3);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op3);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, 0.f);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, 0.f);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op6 + op5);
                float op4 = this->GetOscillatorValue(3);

                return (op1 + op2 + op4) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[1]) + (this->Voice->OperatorLevel[3]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo21 : public DX7Algo<LUTLength, SampleRate>
        {

            float op3Feedback = 0.f;

        public:
            DX7Algo21(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op3Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(2, op3Feedback);
                float op3 = this->GetOscillatorValue(2);
                op3Feedback = op3;

                this->SetOscillatorFrequency(1, op3);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op3);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, 0.f);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op6);
                float op4 = this->GetOscillatorValue(3);

                return (op1 + op2 + op4 + op5) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[1]) + (this->Voice->OperatorLevel[3]) + (this->Voice->OperatorLevel[4]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo22 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo22(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(2, op6);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op6);
                float op4 = this->GetOscillatorValue(3);

                return (op1 + op3 + op4 + op5) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[3]) + (this->Voice->OperatorLevel[4]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo23 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo23(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(0, 0.f);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(2, 0.f);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(1, op3);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op6);
                float op4 = this->GetOscillatorValue(3);

                return (op1 + op2 + op4 + op5) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[1]) + (this->Voice->OperatorLevel[3]) + (this->Voice->OperatorLevel[4]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo24 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo24(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(0, 0.f);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(2, op6);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(3, op6);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                return (op1 + op2 + op3 + op4 + op5) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[1]) + (this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[3]) + (this->Voice->OperatorLevel[4]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo25 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo25(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(0, 0.f);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(2, 0.f);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(3, op6);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                return (op1 + op2 + op3 + op4 + op5) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[1]) + (this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[3]) + (this->Voice->OperatorLevel[4]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo26 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo26(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(0, 0.f);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(2, 0.f);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(1, op3);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, 0.f);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op6 + op5);
                float op4 = this->GetOscillatorValue(3);

                return (op1 + op2 + op4) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[1]) + (this->Voice->OperatorLevel[3]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo27 : public DX7Algo<LUTLength, SampleRate>
        {

            float op3Feedback = 0.f;

        public:
            DX7Algo27(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op3Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(0, 0.f);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(2, op3Feedback);
                float op3 = this->GetOscillatorValue(2);
                op3Feedback = op3;

                this->SetOscillatorFrequency(1, op3);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(5, 0.f);
                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, 0.f);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(3, op6 + op5);
                float op4 = this->GetOscillatorValue(3);

                return (op1 + op2 + op4) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[1]) + (this->Voice->OperatorLevel[3]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo28 : public DX7Algo<LUTLength, SampleRate>
        {

            float op5Feedback = 0.f;

        public:
            DX7Algo28(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op5Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(0, op2);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(5, 0.f);

                float op6 = this->GetOscillatorValue(5);

                this->SetOscillatorFrequency(4, op5Feedback);
                float op5 = this->GetOscillatorValue(4);
                op5Feedback = op5;

                this->SetOscillatorFrequency(3, op5);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4);
                float op3 = this->GetOscillatorValue(2);

                return (op1 + op3 + op6) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[5]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo29 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo29(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(0, 0.f);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(3, 0.f);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                return (op1 + op2 + op3 + op5) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[1]) + (this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[4]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo30 : public DX7Algo<LUTLength, SampleRate>
        {

            float op5Feedback = 0.f;

        public:
            DX7Algo30(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op5Feedback = initFeedback;
            }

            inline float Generate() override
            {

                this->SetOscillatorFrequency(0, 0.f);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(4, op5Feedback);
                float op5 = this->GetOscillatorValue(4);
                op5Feedback = op5;

                this->SetOscillatorFrequency(3, op5);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(2, op4);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(5, 0.f);
                float op6 = this->GetOscillatorValue(5);

                return (op1 + op2 + op3 + op6) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[1]) + (this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[5]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo31 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo31(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }
            inline float Generate() override
            {

                this->SetOscillatorFrequency(0, 0.f);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(2, 0.f);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(3, 0.f);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                this->SetOscillatorFrequency(4, op6);
                float op5 = this->GetOscillatorValue(4);

                return (op1 + op2 + op3 + +op4 + op5) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[1]) + (this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[3]) + (this->Voice->OperatorLevel[4]));
            }
        };

        template <size_t LUTLength, size_t SampleRate>
        class DX7Algo32 : public DX7Algo<LUTLength, SampleRate>
        {

            float op6Feedback = 0.f;

        public:
            DX7Algo32(FMVoice<6, LUTLength, SampleRate> *voice)
                : DX7Algo<LUTLength, SampleRate>(voice)
            {
                this->NormalizeVolume();
            }

            inline void Reset(float initFeedback) override
            {
                op6Feedback = initFeedback;
            }
            inline float Generate() override
            {

                this->SetOscillatorFrequency(0, 0.f);
                float op1 = this->GetOscillatorValue(0);

                this->SetOscillatorFrequency(1, 0.f);
                float op2 = this->GetOscillatorValue(1);

                this->SetOscillatorFrequency(2, 0.f);
                float op3 = this->GetOscillatorValue(2);

                this->SetOscillatorFrequency(3, 0.f);
                float op4 = this->GetOscillatorValue(3);

                this->SetOscillatorFrequency(4, 0.f);
                float op5 = this->GetOscillatorValue(4);

                this->SetOscillatorFrequency(5, op6Feedback);
                float op6 = this->GetOscillatorValue(5);
                op6Feedback = op6;

                return (op1 + op2 + op3 + +op4 + op5 + op6) * this->Voice->Envelopes[6]->CurrentValue() * this->Normalize;
            }

            inline void NormalizeVolume() override
            {
                this->Normalize = 1.f / ((this->Voice->OperatorLevel[0]) + (this->Voice->OperatorLevel[1]) + (this->Voice->OperatorLevel[2]) + (this->Voice->OperatorLevel[3]) + (this->Voice->OperatorLevel[4]) + (this->Voice->OperatorLevel[5]));
            }
        };

    } // namespace Dsp
} // namespace ByteFarm