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

        template <uint8_t NumOscillators, uint16_t LUTSize, size_t SAMPLERATE>
        TypedArray<Voice *, 1, uint8_t> *GetVoices()
        {
            TypedArray<Voice *, 1, uint8_t> *voices = new TypedArray<Voice *, 1, uint8_t>();

            FMVoice<NumOscillators, LUTSize, SAMPLERATE> *v = new FMVoice<NumOscillators, LUTSize, SAMPLERATE>();

            for (uint8_t i = 0; i < NumOscillators; i++)
            {
                //Envelope<SAMPLERATE> *env = new Envelope<SAMPLERATE>((Attack | Decay | Sustain | Release), 200.f, 3000.f * i, 2000.f, 500.f * i, 5000.f * i, 0.75f);
                Envelope<SAMPLERATE> *env = new Envelope<SAMPLERATE>(Delay | Attack | Hold | Decay | Sustain | Release, //envelope segments
                                                                     0.f + 200.f * fabs(osc_white()),                   //delay
                                                                     5.f + 2000.f * fabs(osc_white()),                  //attack
                                                                     0.f + 400.f * fabs(osc_white()),                   //hold
                                                                     2.f + 2000.f * fabs(osc_white()),                  //decay
                                                                     2.f + 4000.f * fabs(osc_white()),                  //release
                                                                     0.5f + 0.5f * osc_white(),                         //sustain level
                                                                     0.2f);                                             //slop

                v->Envelopes[i] = env;
            }

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

            virtual void UpdateParams(uint16_t index, uint16_t value) override
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
                    }
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
                    //voice->AmpModulator = param_val_to_f32(value) * 0.5f + 0.5f;
                    break;
                }
                case k_user_osc_param_id4:
                {
                    voice->PitchOffset = value - 100;
                    break;
                }
                case k_user_osc_param_id5:
                {
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

            virtual void UpdateOscParams(VoiceParams params) override
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