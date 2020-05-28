#pragma once
#include "OscModule.hpp"
#include "LUTVoice.hpp"
#include "LUTOsc.hpp"
//#include "LUT1024.hpp"
#include "LUT.hpp"
#include "../tools/WaveTable/WaveTableGenerator.hpp"
#include "userosc.h"

namespace ByteFarm
{
    namespace Dsp
    {

        template <uint8_t NumOscillators, uint8_t NumEnvelopes, size_t LUTSize, size_t SampleRate>
        class WaveTableOneLUTVoice : public LUTVoice<NumOscillators, NumEnvelopes, LUTSize, SampleRate>
        {
        public:
            WaveTableOneLUTVoice(float slop, OscillatorAmplitudeCalulator<NumOscillators> *oscAmpCalc, OscillatorTuningModifier<NumOscillators> *tuningModifier)
                : LUTVoice<NumOscillators, NumEnvelopes, LUTSize, SampleRate>(slop, oscAmpCalc, tuningModifier)
            {
            }

            virtual void UpdateOscParams(VoiceParams params) override
            {
                this->Params = params;
                float freq = this->NoteToHz(this->TuningModifier->GetOscillatorNote(0, params.NoteNumber), params.Detune);
                float logFreq = log2f(freq);

                this->Oscillators.Get(0)->SetFreq(freq + (logFreq * osc_white() * this->Slop));
                this->Oscillators.Get(1)->SetFreq(freq + (logFreq * osc_white() * this->Slop));
                this->Oscillators.Get(2)->SetFreq(freq + (logFreq * osc_white() * this->Slop));
            };
        };

        template <size_t LUTSize, size_t SampleRate>
        TypedArray<Voice *, 1, uint8_t> *GetVoices()
        {
            TypedArray<Voice *, 1, uint8_t> *arr = new TypedArray<Voice *, 1, uint8_t>();

            LUT<LUTSize> *sqr = new LUT<LUTSize>();
            LUT<LUTSize> *saw = new LUT<LUTSize>();
            LUT<LUTSize> *tri = new LUT<LUTSize>();

            ByteFarm::Tools::WaveTableGenerator::Fill<LUTSize>(sqr->Table, ByteFarm::Tools::CommonWaveTableFunctors::Square<8>);
            ByteFarm::Tools::WaveTableGenerator::Fill<LUTSize>(saw->Table, ByteFarm::Tools::CommonWaveTableFunctors::Saw<8>);
            ByteFarm::Tools::WaveTableGenerator::Fill<LUTSize>(tri->Table, ByteFarm::Tools::CommonWaveTableFunctors::Tri<8>);

            LUTOsc<LUTSize, SampleRate> *sqrOsc = new LUTOsc<LUTSize, SampleRate>(sqr->Table, osc_white());
            LUTOsc<LUTSize, SampleRate> *sawOsc = new LUTOsc<LUTSize, SampleRate>(saw->Table, osc_white());
            LUTOsc<LUTSize, SampleRate> *triOsc = new LUTOsc<LUTSize, SampleRate>(tri->Table, osc_white());

            OscillatorAmplitudeCalulator<3> *oscAmp = new PhasedAmplitudeCalculator<3>();
            OscillatorTuningModifier<3> *oscTune = new UnisonTuningModifier<3>(-12);

            LUTVoice<3, 3, LUTSize, SampleRate> *v = new WaveTableOneLUTVoice<3, 3, LUTSize, SampleRate>(0.1f, oscAmp, oscTune);

            v->Oscillators.Set(0, sqrOsc);
            v->Oscillators.Set(1, sawOsc);
            v->Oscillators.Set(2, triOsc);

            Envelope<SampleRate> *sqrEnv = new Envelope<SampleRate>(Delay | Attack | Hold | Decay | Sustain | Release, 5.f, 2.f, 100.f, 100.f, 5000.f, 0.85f, 0.05f);
            Envelope<SampleRate> *sawEnv = new Envelope<SampleRate>(Delay | Attack | Hold | Decay | Release | Loop, 75.f, 300.f, 400.f, 7000.f, 5000.f, 0.75f, 0.05f);
            Envelope<SampleRate> *triEnv = new Envelope<SampleRate>(Delay | Attack | Hold | Decay | Release | Loop, 100.f, 4000.f, 200.f, 2000.f, 1000.f, 0.65f, 0.05f);

            v->Envelopes.Set(0, sqrEnv);
            v->Envelopes.Set(1, sawEnv);
            v->Envelopes.Set(2, triEnv);

            arr->Set(0, v);
            return arr;
        };

        template <size_t SAMPLERATE>
        class WaveTableOneModule : public OscModule<1, SAMPLERATE>
        {

        public:
            WaveTableOneModule() : OscModule<1, SAMPLERATE>(GetVoices<1024, SAMPLERATE>())
            {
            }

            virtual void UpdateParams(uint16_t index, uint16_t value) override
            {

                WaveTableOneLUTVoice<3, 3, 1024, SAMPLERATE> *voice = (WaveTableOneLUTVoice<3, 3,1024, SAMPLERATE> *)this->Voices->Get(0);

                switch (index)
                {
                case k_user_osc_param_id1:
                {
                    voice->SetSlop((float)value/100.f);
                    break;
                }
                case k_user_osc_param_id2:
                {
                    for(uint8_t i = 0;i <voice->Envelopes.Size();i++)
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
    } // namespace Dsp
} // namespace ByteFarm