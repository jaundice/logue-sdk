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

        template <uint8_t NumOscillators, uint8_t NumAlgorithms, uint16_t LUTSize, size_t SAMPLERATE>
        TypedArray<Voice *, 1, uint8_t> *GetVoices()
        {
            TypedArray<Voice *, 1, uint8_t> *voices = new TypedArray<Voice *, 1, uint8_t>();

            FMVoice<NumOscillators, NumAlgorithms, LUTSize, SAMPLERATE> *v = new FMVoice<NumOscillators, NumAlgorithms, LUTSize, SAMPLERATE>();

            for (uint8_t i = 0; i < NumOscillators; i++)
            {
                //Envelope<SAMPLERATE> *env = new Envelope<SAMPLERATE>((Attack | Decay | Sustain | Release), 200.f, 3000.f * i, 2000.f, 500.f * i, 5000.f * i, 0.75f);
                Envelope<SAMPLERATE> *env = new Envelope<SAMPLERATE>(Delay | Attack | Hold | Decay | Sustain | Release | Loop, //envelope segments
                                                                     1.f + 3.f * fabs(osc_white()),                            //delay
                                                                     2.f + 50.f * fabs(osc_white()),                           //attack
                                                                     200.f + 400.f * fabs(osc_white()),                        //hold
                                                                     20.f + 100.f * fabs(osc_white()),                         //decay
                                                                     4000.f + 1000.f * fabs(osc_white()),                      //release
                                                                     0.5f + 0.5f * fabs(osc_white()),                          //sustain level
                                                                     0.05f);                                                   //slop

                //&(v->GetEnvelope(i)) =  *env;
            }

            voices->Set(0, v);
            return voices;
        };

        template <size_t SAMPLERATE, size_t NumOscillators, size_t NumAlgorithms, size_t LUTSize>
        class FMModule : public OscModule<1, SAMPLERATE>
        {

        public:
            FMModule() : OscModule<1, SAMPLERATE>(GetVoices<NumOscillators, 1, LUTSize, SAMPLERATE>())
            {
            }

            virtual void UpdateParams(uint16_t index, uint16_t value) override
            {

                FMVoice<NumOscillators, NumAlgorithms, LUTSize, SAMPLERATE> *voice = (FMVoice<NumOscillators, NumAlgorithms, LUTSize, SAMPLERATE> *)this->Voices->Get(0);

                switch (index)
                {
                case k_user_osc_param_id1:
                {
                    //voice->SetSlop(param_val_to_f32(value) * 0.5f + 0.5f);
                    break;
                }
                case k_user_osc_param_id2:
                {
                    //for (uint8_t i = 0; i < voice->Envelopes.Size(); i++)
                    //{
                    //    voice->Envelopes.Get(i)->SetSlop(param_val_to_f32(value) * 0.5f + 0.5f);
                    //}
                    break;
                }
                case k_user_osc_param_id3:
                {
                    //voice->AmpModulator = param_val_to_f32(value) * 0.5f + 0.5f;
                    break;
                }
                case k_user_osc_param_id4:
                {
                    //voice->TuningModifier->SetSemis(value -100);
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

        template<size_t SAMPLERATE>
        class SixOpFMModule : public FMModule<SAMPLERATE, 6, 1, 1024>{
            public:
            SixOpFMModule() :FMModule<SAMPLERATE, 6, 1, 1024>(){
                FMVoice<6, 1, 1024, SAMPLERATE> * fmv = (FMVoice<6, 1, 1024, SAMPLERATE>*)this->Voices->Get(0);
                fmv->SetAlgorithm(0, new DX7Algo1<1024, SAMPLERATE>(fmv));

                for(uint8_t i=0;i<6;i++){
                     fmv->OscRatio[i] = 0.25f *(1.f + (float)i);
                     fmv->OperatorLevel[i] = 1.f - 0.1f/(float)i;
                 }
            }
        } ;

    } // namespace Dsp
} // namespace ByteFarm