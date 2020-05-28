#pragma once
#include "OscModule.hpp"
#include "TypedArray.hpp"
#include "LUT.hpp"
#include "LUTOsc.hpp"
#include "LUTVoice.hpp"
#include "../tools/WaveTable/WaveTableGenerator.hpp"
#include "Voice.hpp"
#include "Envelope.hpp"
#include "userosc.h"

namespace ByteFarm
{
    namespace Dsp
    {

        template <uint8_t NumOscillators, uint8_t NumEnvelopes, uint16_t LUTSize, size_t SAMPLERATE>
        TypedArray<Voice *, 1, uint8_t> *GetVoices(ByteFarm::Tools::CommonWaveShapes waveType)
        {
            TypedArray<Voice *, 1, uint8_t> *voices = new TypedArray<Voice *, 1, uint8_t>();
            LUT<LUTSize> *table = new LUT<LUTSize>();

            ByteFarm::Tools::Functor functor;
            switch (waveType)
            {
            case ByteFarm::Tools::CommonWaveShapes::Saw:
            {
                functor = ByteFarm::Tools::CommonWaveTableFunctors::Saw<8>;
                break;
            }
            case ByteFarm::Tools::CommonWaveShapes::Tri:
            {
                functor = ByteFarm::Tools::CommonWaveTableFunctors::Tri<8>;
                break;
            }
            case ByteFarm::Tools::CommonWaveShapes::Sqr:
            {
                functor = ByteFarm::Tools::CommonWaveTableFunctors::Square<8>;
                break;
            }
            }

            ByteFarm::Tools::WaveTableGenerator::Fill<LUTSize>(table->Table, functor);

            OscillatorAmplitudeCalulator<NumOscillators> *oscAmp = new EqualAmplitudeCalculator<NumOscillators>();

            OscillatorTuningModifier<NumOscillators> *oscTune = new OctavesTuningModifier<NumOscillators, 3>(-12);

            LUTVoice<NumOscillators, NumEnvelopes, LUTSize, SAMPLERATE> *v = new LUTVoice<NumOscillators, NumEnvelopes, LUTSize, SAMPLERATE>(0.01f, oscAmp, oscTune);

            for (uint8_t i = 0; i < NumOscillators; i++)
            {
                LUTOsc<LUTSize, SAMPLERATE> *osc = new LUTOsc<LUTSize, SAMPLERATE>(table->Table, osc_white());
                v->Oscillators.Set(i, osc);
            }

            for (uint8_t i = 0; i < NumEnvelopes; i++)
            {
                //Envelope<SAMPLERATE> *env = new Envelope<SAMPLERATE>((Attack | Decay | Sustain | Release), 200.f, 3000.f * i, 2000.f, 500.f * i, 5000.f * i, 0.75f);
                Envelope<SAMPLERATE> *env = new Envelope<SAMPLERATE>(Delay | Attack | Hold | Decay | Sustain | Release, //envelope segments
                                                                     1.f + 3.f * fabs(osc_white()),                     //delay
                                                                     2.f + 50.f * fabs(osc_white()),                    //attack
                                                                     200.f + 400.f * fabs(osc_white()),                 //hold
                                                                     20.f + 100.f * fabs(osc_white()),                  //decay
                                                                     4000.f + 1000.f * fabs(osc_white()),               //release
                                                                     0.5f + 0.5f * fabs(osc_white()),                   //sustain level
                                                                     0.05f);                                            //slop

                v->Envelopes.Set(i, env);
            }

            voices->Set(0, v);
            return voices;
        };

        template <size_t SAMPLERATE>
        class SuperOscModule : public OscModule<1, SAMPLERATE>
        {

        public:
            SuperOscModule(ByteFarm::Tools::CommonWaveShapes waveType) : OscModule<1, SAMPLERATE>(GetVoices<16, 16, 1024, SAMPLERATE>(waveType))
            {
            }

            virtual void UpdateParams(uint16_t index, uint16_t value) override
            {

                LUTVoice<16, 16, 1024, SAMPLERATE> *voice = (LUTVoice<16, 16, 1024, SAMPLERATE> *)this->Voices->Get(0);

                switch (index)
                {
                case k_user_osc_param_id1:
                {
                    voice->SetSlop((float)value/100.f);
                    break;
                }
                case k_user_osc_param_id2:
                {
                    for (uint8_t i = 0; i < voice->Envelopes.Size(); i++)
                    {
                        voice->Envelopes.Get(i)->SetSlop((float)value/100.f);
                    }
                    break;
                }
                case k_user_osc_param_id3:
                {
                    voice->AmpModulator = (float)value/100.f;
                    break;
                }
                case k_user_osc_param_id4:
                {
                    voice->TuningModifier->SetSemis(value);
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
        template <size_t SAMPLERATE>
        class SuperSawModule : public SuperOscModule<SAMPLERATE>
        {

        public:
            SuperSawModule() : SuperOscModule<SAMPLERATE>(ByteFarm::Tools::CommonWaveShapes::Saw)
            {
            }
        };

        template <size_t SAMPLERATE>
        class SuperTriModule : public SuperOscModule<SAMPLERATE>
        {

        public:
            SuperTriModule() : SuperOscModule<SAMPLERATE>(ByteFarm::Tools::CommonWaveShapes::Tri)
            {
            }
        };

        template <size_t SAMPLERATE>
        class SuperSquareModule : public SuperOscModule<SAMPLERATE>
        {

        public:
            SuperSquareModule() : SuperOscModule<SAMPLERATE>(ByteFarm::Tools::CommonWaveShapes::Sqr)
            {
            }
        };

    } // namespace Dsp
} // namespace ByteFarm