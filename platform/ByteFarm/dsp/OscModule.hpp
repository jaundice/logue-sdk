#pragma once
#include "TypedArray.hpp"
#include "Voice.hpp"
#include <inttypes.h>
#include "osc_api.h"

namespace ByteFarm
{
    namespace Dsp
    {

        template <uint8_t NumVoices, size_t SAMPLERATE>
        class OscModule
        {
            protected:
            TypedArray<Voice *, NumVoices, uint8_t> *Voices;

        public:
            OscModule(TypedArray<Voice *, NumVoices, uint8_t> *voices)
            {
                Voices = voices;
            }

            inline virtual VoiceParams ConvertOscParams(const user_osc_param_t *const params)
            {
                VoiceParams vp;
                vp.NoteNumber = params->pitch >> 8;
                vp.Detune = (float)(params->pitch & 0xFFFF) * k_note_mod_fscale;
                vp.Cutoff = (float)params->cutoff / 0x1fff;
                vp.Resonance = (float)params->resonance / 0x1fff;
                vp.ShapeLfo = q31_to_f32(params->shape_lfo);
                return vp;
            };

            inline virtual void Reset()
            {
                for (uint8_t i = 0; i < NumVoices; i++)
                {
                    Voices->Get(i)->Reset();
                }
            }

            inline virtual void Increment(uint32_t frames)
            {
                for (uint8_t i = 0; i < NumVoices; i++)
                {
                    Voices->Get(i)->IncrementEnvelope(frames);
                }
            }

            inline virtual void NoteOn()
            {
                for (uint8_t i = 0; i < NumVoices; i++)
                {
                    Voices->Get(i)->NoteOn();
                }
            }

            inline virtual void NoteOff()
            {
                for (uint8_t i = 0; i < NumVoices; i++)
                {
                    Voices->Get(i)->NoteOff();
                }
            }

            virtual void UpdateOscParams(VoiceParams params) = 0;

            inline void Generate(int32_t *yn,
                          const uint32_t frames)
            {
                q31_t *__restrict y = (q31_t *)yn;
                const q31_t *y_e = y + frames;

                for (; y != y_e;)
                {
                    float sig = 0.f;
                    for (uint8_t i = 0; i < NumVoices; i++)
                    {
                        sig += this->Voices->Get(i)->Generate();
                    }
                    *(y++) = f32_to_q31(sig);
                }
                
                Increment(frames);
            }

            virtual void UpdateParams(uint16_t index, uint16_t value) = 0;

            ~OscModule()
            {
            }
        };
    } // namespace Dsp
} // namespace ByteFarm