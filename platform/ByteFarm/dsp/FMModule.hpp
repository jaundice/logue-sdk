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

        inline EnvelopeStage RandomEnvelope()
        {
            EnvelopeStage stages = Attack | Decay;

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
                stages = stages | Release;
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

            for (uint8_t i = 0; i < NumOscillators; i++)
            {
                //Envelope<SAMPLERATE> *env = new Envelope<SAMPLERATE>((Attack | Decay | Sustain | Release), 200.f, 3000.f * i, 2000.f, 500.f * i, 5000.f * i, 0.75f);
                Envelope<SAMPLERATE> *env = new Envelope<SAMPLERATE>(RandomEnvelope(),                    //envelope segments
                                                                     0.f + 200.f * powf(osc_white(), 2),  //delay
                                                                     6.f + 2000.f * powf(osc_white(), 2), //attack
                                                                     0.f + 400.f * powf(osc_white(), 2),  //hold
                                                                     6.f + 2000.f * powf(osc_white(), 2), //decay
                                                                     6.f + 4000.f * powf(osc_white(), 2), //release
                                                                     0.5f + 0.5f * osc_white(),           //sustain level
                                                                     0.2f);                               //slop

                v->Envelopes[i] = env;
            }

            v->Modulate = osc_white() < 1 ? ModulateFreqAndAdd : ModulateFreq;

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
                    if (value == voice->GetAlgorithmIndex())
                        break;

                    switch (value)
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
            }
        };

    } // namespace Dsp
} // namespace ByteFarm