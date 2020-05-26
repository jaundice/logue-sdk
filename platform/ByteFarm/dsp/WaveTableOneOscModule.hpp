#pragma once
#include "OscModule.hpp"
#include "LUTVoice.hpp"
#include "LUTOsc.hpp"
//#include "LUT1024.hpp"
#include "LUT.hpp"
#include "../tools/WaveTable/WaveTableGenerator.hpp"

namespace ByteFarm
{
    namespace Dsp
    {

        template <uint8_t NumOscillators, uint8_t numEnvelopes, size_t LUTSize, size_t SampleRate>
        class WaveTableOneLUTVoice : public LUTVoice<NumOscillators, numEnvelopes, LUTSize, SampleRate>
        {
        public:
            inline virtual float OscillatorAmpModifier(uint8_t oscillatorIndex)
            {
                float scale = 0.5f;
                if (oscillatorIndex == 1)
                {
                    scale = 0.25f + this->Params.ShapeLfo * 0.5f;
                }
                else if (oscillatorIndex == 2)
                {
                    scale = 0.25f + (1.f - this->Params.ShapeLfo) * 0.5f;
                }

                return scale;
            }

            inline virtual void UpdateOscParams(VoiceParams params) override
            {
                this->Params = params;
                float freq = this->NoteToHz(params.NoteNumber, params.Detune);
                float logFreq = log2f(freq);
                
                this->Oscillators.Get(0)->SetFreq(freq + logFreq * osc_white());
                this->Oscillators.Get(1)->SetFreq(freq + logFreq * -1.f * osc_white());
                this->Oscillators.Get(2)->SetFreq(freq + logFreq * osc_white());
            };
        };

        template <size_t LUTSize, size_t SampleRate>
        TypedArray<Voice *, 1, uint8_t> *GetVoices()
        {
            TypedArray<Voice *, 1, uint8_t> *arr = new TypedArray<Voice *, 1, uint8_t>();

            LUT<LUTSize> *sqr = new LUT<LUTSize>();
            LUT<LUTSize> *saw = new LUT<LUTSize>();
            LUT<LUTSize> *tri = new LUT<LUTSize>();
            //LUT<LUTSize> *sin = new LUT<LUTSize>();

            ByteFarm::Tools::WaveTableGenerator::Fill<LUTSize>(sqr->Table, ByteFarm::Tools::CommonWaveTableFunctors::Square<8>);
            ByteFarm::Tools::WaveTableGenerator::Fill<LUTSize>(saw->Table, ByteFarm::Tools::CommonWaveTableFunctors::Saw<8>);
            ByteFarm::Tools::WaveTableGenerator::Fill<LUTSize>(tri->Table, ByteFarm::Tools::CommonWaveTableFunctors::Tri<8>);
            //ByteFarm::Tools::WaveTableGenerator::Fill<LUTSize>(sin->Table, ByteFarm::Tools::CommonWaveTableFunctors::Sine);

            LUTOsc<LUTSize, SampleRate> *sqrOsc = new LUTOsc<LUTSize, SampleRate>(sqr->Table, osc_white());
            LUTOsc<LUTSize, SampleRate> *sawOsc = new LUTOsc<LUTSize, SampleRate>(saw->Table, osc_white());
            LUTOsc<LUTSize, SampleRate> *triOsc = new LUTOsc<LUTSize, SampleRate>(tri->Table, osc_white());
            //LUTOsc<LUTSize, SampleRate> *sinOsc = new LUTOsc<LUTSize, SampleRate>(sin->Table);

            LUTVoice<3, 3, LUTSize, SampleRate> *v = new WaveTableOneLUTVoice<3, 3, LUTSize, SampleRate>();

            v->Oscillators.Set(0, sqrOsc);
            v->Oscillators.Set(1, sawOsc);
            v->Oscillators.Set(2, triOsc);
            //v->Oscillators.Set(3, sinOsc);

            Envelope<SampleRate> *sqrEnv = new Envelope<SampleRate>(Delay | Attack | Hold | Decay | Sustain | Release, 5.f, 2.f, 100.f, 100.f, 5000.f, 0.85f);
            Envelope<SampleRate> *sawEnv = new Envelope<SampleRate>(Delay | Attack | Hold | Decay | Release | Loop, 75.f, 300.f, 400.f, 7000.f, 5000.f, 0.75f);
            Envelope<SampleRate> *triEnv = new Envelope<SampleRate>(Delay | Attack | Hold | Decay | Release | Loop, 100.f, 4000.f, 200.f, 2000.f, 1000.f, 0.65f);
            //Envelope<SampleRate> *sinEnv = new Envelope<SampleRate>(Delay|Attack|Hold|Decay|Release|Loop, 100.f, 50.f, 2.f, 2000.f, 1000.f, 0.45f);

            v->Envelopes.Set(0, sqrEnv);
            v->Envelopes.Set(1, sawEnv);
            v->Envelopes.Set(2, triEnv);
            //v->Envelopes.Set(3, sinEnv);

            arr->Set(0, v);
            return arr;
        };

        class WaveTableOneModule : public OscModule<1>
        {

        public:
            WaveTableOneModule() : OscModule<1>(GetVoices<1024, 48000>())
            {
            }

            virtual void UpdateParams(uint16_t index, uint16_t value) override{

            };

            virtual void UpdateOscParams(VoiceParams params) override
            {
                for (uint8_t i = 0; i < Voices->Size(); i++)
                {
                    Voices->Get(i)->UpdateOscParams(params);
                }
            };
        };
    } // namespace Dsp
} // namespace ByteFarm