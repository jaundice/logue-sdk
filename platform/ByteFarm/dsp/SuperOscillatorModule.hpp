#pragma once
#include "OscModule.hpp"
#include "TypedArray.hpp"
#include "LUT.hpp"
#include "LUTOsc.hpp"
#include "LUTVoice.hpp"
#include "../tools/WaveTable/WaveTableGenerator.hpp"
#include "Voice.hpp"
#include "Envelope.hpp"

namespace ByteFarm
{
    namespace Dsp
    {

        template <uint8_t numOscilators, uint8_t numEnvelopes, uint16_t LUTSize, size_t SAMPLERATE>
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

            LUTVoice<numOscilators, numEnvelopes, LUTSize, SAMPLERATE> *v = new LUTVoice<numOscilators, numEnvelopes, LUTSize, SAMPLERATE>();

            for (uint8_t i = 0; i < numOscilators; i++)
            {
                LUTOsc<LUTSize, SAMPLERATE> *osc = new LUTOsc<LUTSize, SAMPLERATE>(table->Table, osc_white());
                v->Oscillators.Set(i, osc);
            }

            for (uint8_t i = 0; i < numEnvelopes; i++)
            {
                //Envelope<SAMPLERATE> *env = new Envelope<SAMPLERATE>((Attack | Decay | Sustain | Release), 200.f, 3000.f * i, 2000.f, 500.f * i, 5000.f * i, 0.75f);
                Envelope<SAMPLERATE> *env = new Envelope<SAMPLERATE>(Delay | Attack | Hold | Decay | Sustain | Release, //envelope segments
                                                                     1.f + 3.f * fabs(osc_white()),//delay
                                                                     2.f + 50.f * fabs(osc_white()),//attack
                                                                     200.f + 400.f * fabs(osc_white()),//hold
                                                                     20.f + 100.f * fabs(osc_white()),//decay
                                                                     4000.f + 1000.f * fabs(osc_white()),//release
                                                                     0.5f + 0.5f * fabs(osc_white()));//sustain level

                v->Envelopes.Set(i, env);
            }

            voices->Set(0, v);
            return voices;
        };

        class SuperOscModule : public OscModule<1>
        {

        public:
            SuperOscModule(ByteFarm::Tools::CommonWaveShapes waveType) : OscModule<1>(GetVoices<16, 16, 1024, 48000>(waveType))
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

        class SuperSawModule : public SuperOscModule
        {

        public:
            SuperSawModule() : SuperOscModule(ByteFarm::Tools::CommonWaveShapes::Saw)
            {
            }
        };

        class SuperTriModule : public SuperOscModule
        {

        public:
            SuperTriModule() : SuperOscModule(ByteFarm::Tools::CommonWaveShapes::Tri)
            {
            }
        };

        class SuperSquareModule : public SuperOscModule
        {

        public:
            SuperSquareModule() : SuperOscModule(ByteFarm::Tools::CommonWaveShapes::Sqr)
            {
            }
        };

    } // namespace Dsp
} // namespace ByteFarm