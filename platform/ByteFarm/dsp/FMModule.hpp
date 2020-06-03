#pragma once
#include "FMVoice.hpp"
#include "TypedArray.hpp"
#include "LUTVoice.hpp"
#include "Envelope.hpp"
#include "OscModule.hpp"
#include "userosc.h"

namespace ByteFarm
{
    namespace Dsp
    {

        float DX100Ratios[64]{0.50f, 0.71f, 0.78f, 0.87f, 1.00f, 1.41f, 1.57f, 1.73f, 2.00f, 2.82f, 3.00f, 3.14f, 3.46f, 4.00f, 4.24f, 4.71f, 5.00f, 5.19f, 5.65f, 6.00f, 6.28f, 6.92f, 7.00f, 7.07f, 7.85f, 8.00f, 8.48f, 8.65f, 9.00f, 9.42f, 9.89f, 10.00f, 10.38f, 10.99f, 11.00f, 11.30f, 12.00f, 12.11f, 12.56f, 12.72f, 13.00f, 13.84f, 14.00f, 14.10f, 14.13f, 15.00f, 15.55f, 15.57f, 15.70f, 16.96f, 17.27f, 17.30f, 18.37f, 18.84f, 19.03f, 19.78f, 20.41f, 20.76f, 21.20f, 21.98f, 22.49f, 23.55f, 24.22f, 25.95f};

        inline EnvelopeStage RandomEnvelope(bool crossFade)
        {
            EnvelopeStage stages = Attack | Decay | Release;

            if (crossFade)
                stages = stages | Crossfade;

            if (osc_white() > 0)
            {
                stages = stages | Delay;
            }
            if (osc_white() > 0)
            {
                stages = stages | Hold;
            }
            if (osc_white() > 0)
            {
                stages = stages | Sustain;
            }
            if (osc_white() > 0)
            {
                stages = stages | Loop;
            }

            return stages;
        }

        template <uint8_t NumOscillators, uint16_t LUTSize, size_t SAMPLERATE>
        TypedArray<Voice *, 1, uint8_t> *GetVoices()
        {
            TypedArray<Voice *, 1, uint8_t> *voices = new TypedArray<Voice *, 1, uint8_t>();

            FMVoice<NumOscillators, LUTSize, SAMPLERATE> *v = new FMVoice<NumOscillators, LUTSize, SAMPLERATE>();

            for (uint8_t i = 0; i < NumOscillators + 1; i++)
            {
                //Envelope<SAMPLERATE> *env = new Envelope<SAMPLERATE>((Attack | Decay | Sustain | Release), 200.f, 3000.f * i, 2000.f, 500.f * i, 5000.f * i, 0.75f);
                Envelope<SAMPLERATE> *env = new Envelope<SAMPLERATE>(RandomEnvelope(i == NumOscillators),   //envelope segments
                                                                     0.f + 200.f * powf(osc_white(), 2.f),  //delay
                                                                     1.f + 2000.f * powf(osc_white(), 2.f), //attack
                                                                     0.f + 2000.f * powf(osc_white(), 2.f), //hold
                                                                     1.f + 2000.f * powf(osc_white(), 2.f), //decay
                                                                     1.f + 2000.f * powf(osc_white(), 2.f), //release
                                                                     0.5f + 0.5f * osc_white(),             //sustain level
                                                                     0.2f);                                 //slop

                v->Envelopes[i] = env;
            }

            v->Modulate = osc_white() < 1.f ? ModulateFreqAndAdd : ModulateFreq;

            voices->Set(0, v);
            return voices;
        };

        template <uint8_t NumOscillators, size_t SAMPLERATE, size_t LUTSize>
        class FMModule : public OscModule<1, SAMPLERATE>
        {

        public:
            FMModule() : OscModule<1, SAMPLERATE>(GetVoices<NumOscillators, LUTSize, SAMPLERATE>())
            {
            }

            inline virtual void UpdateParams(uint16_t index, uint16_t value) override
            {

                FMVoice<NumOscillators, LUTSize, SAMPLERATE> *voice = (FMVoice<NumOscillators, LUTSize, SAMPLERATE> *)this->Voices->Get(0);

                switch (index)
                {
                case k_user_osc_param_id1:
                {

                    //voice->Modulate = ModulateFreqAndAdd;
                    break;
                }
                case k_user_osc_param_id2:
                {
                    for (uint8_t i = 0; i < NumOscillators + 1; i++)
                    {
                        voice->GetEnvelope(i)->SetSlop(param_val_to_f32(value) * 0.5f + 0.5f);
                    }
                    break;
                }
                case k_user_osc_param_id3:
                {
                    voice->SetModulator(value == 0 ? ModulateFreqAndAdd : ModulateFreq);
                    break;
                }
                case k_user_osc_param_id4:
                {
                    voice->PitchOffset = value - 100;
                    break;
                }
                case k_user_osc_param_id5:
                {
                    voice->ResetOscillatorsOnNoteOn = value == 0 ? false : true;
                    break;
                }
                case k_user_osc_param_id6:
                {
                    break;
                }
                case k_user_osc_param_shape:
                {
                    uint16_t algo = (uint16_t)fminf(floorf((32.f) * (float)value / (float)1023), 31);

                    if (algo == voice->GetAlgorithmIndex())
                        break;

                    switch (algo)
                    {
                    case 0:
                        voice->SetAlgorithm(0, new DX7Algo1<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 1:
                        voice->SetAlgorithm(1, new DX7Algo2<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 2:
                        voice->SetAlgorithm(2, new DX7Algo3<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 3:
                        voice->SetAlgorithm(3, new DX7Algo4<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 4:
                        voice->SetAlgorithm(4, new DX7Algo5<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 5:
                        voice->SetAlgorithm(5, new DX7Algo6<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 6:
                        voice->SetAlgorithm(6, new DX7Algo7<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 7:
                        voice->SetAlgorithm(7, new DX7Algo8<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 8:
                        voice->SetAlgorithm(8, new DX7Algo9<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 9:
                        voice->SetAlgorithm(9, new DX7Algo10<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 10:
                        voice->SetAlgorithm(10, new DX7Algo11<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 11:
                        voice->SetAlgorithm(11, new DX7Algo12<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 12:
                        voice->SetAlgorithm(12, new DX7Algo13<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 13:
                        voice->SetAlgorithm(13, new DX7Algo14<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 14:
                        voice->SetAlgorithm(14, new DX7Algo15<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 15:
                        voice->SetAlgorithm(15, new DX7Algo16<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 16:
                        voice->SetAlgorithm(16, new DX7Algo17<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 17:
                        voice->SetAlgorithm(17, new DX7Algo18<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 18:
                        voice->SetAlgorithm(18, new DX7Algo19<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 19:
                        voice->SetAlgorithm(19, new DX7Algo20<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 20:
                        voice->SetAlgorithm(20, new DX7Algo21<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 21:
                        voice->SetAlgorithm(21, new DX7Algo22<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 22:
                        voice->SetAlgorithm(22, new DX7Algo23<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 23:
                        voice->SetAlgorithm(23, new DX7Algo24<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 24:
                        voice->SetAlgorithm(24, new DX7Algo25<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 25:
                        voice->SetAlgorithm(25, new DX7Algo26<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 26:
                        voice->SetAlgorithm(26, new DX7Algo27<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 27:
                        voice->SetAlgorithm(27, new DX7Algo28<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 28:
                        voice->SetAlgorithm(28, new DX7Algo29<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 29:
                        voice->SetAlgorithm(29, new DX7Algo30<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 30:
                        voice->SetAlgorithm(30, new DX7Algo31<LUTSize, SAMPLERATE>(voice));
                        break;
                    case 31:
                        voice->SetAlgorithm(31, new DX7Algo32<LUTSize, SAMPLERATE>(voice));
                        break;
                    }

                    break;
                }
                case k_user_osc_param_shiftshape:
                {
                    break;
                }
                }
            };

            inline virtual void UpdateOscParams(VoiceParams params) override
            {
                for (uint8_t i = 0; i < this->Voices->Size(); i++)
                {
                    this->Voices->Get(i)->UpdateOscParams(params);
                }
            };
        };

        template <size_t SAMPLERATE, size_t LUTSize>
        class SixOpFMModule : public FMModule<6, SAMPLERATE, LUTSize>
        {
        public:
            SixOpFMModule() : FMModule<6, SAMPLERATE, LUTSize>()
            {
                FMVoice<6, LUTSize, SAMPLERATE> *fmv = (FMVoice<6, LUTSize, SAMPLERATE> *)this->Voices->Get(0);

                for (uint8_t i = 0; i < 6; i++)
                {
                    fmv->OscRatio[i] = (10.f * osc_white());
                    fmv->OperatorLevel[i] = 1.f * osc_white(); //- 0.1f / (float)i;
                }

                fmv->SetAlgorithm(0, new DX7Algo1<LUTSize, SAMPLERATE>(fmv));
                fmv->SetModulator(ModulateFreqAndAdd);
            }
        };

        template <size_t SAMPLERATE, size_t LUTSize>
        class DX100Module : public FMModule<6, SAMPLERATE, LUTSize>
        {
        public:
            DX100Module() : FMModule<6, SAMPLERATE, LUTSize>()
            {
                FMVoice<6, LUTSize, SAMPLERATE> *fmv = (FMVoice<6, LUTSize, SAMPLERATE> *)this->Voices->Get(0);

                for (uint8_t i = 0; i < 6; i++)
                {
                    fmv->OscRatio[i] = DX100Ratios[(uint8_t)floorf(64.f * osc_white())];
                    fmv->OperatorLevel[i] = 1.f * osc_white(); //- 0.1f / (float)i;
                }

                fmv->SetAlgorithm(0, new DX7Algo1<LUTSize, SAMPLERATE>(fmv));
                fmv->SetModulator(ModulateFreqAndAdd);
                //fmv->ResetOscillators();
            }
        };

    } // namespace Dsp
} // namespace ByteFarm