#pragma once
#include <type_traits>
namespace ByteFarm
{
    namespace Dsp
    {

        struct VoiceParams
        {
            uint8_t NoteNumber;
            float Detune;
            /** Value of LFO implicitely applied to shape parameter */
            float ShapeLfo;
            /** Current pitch. high byte: note number, low byte: fine (0-255) */
            uint16_t pitch;
            /** Current cutoff value (0x0000-0x1fff) */
            float Cutoff;
            /** Current resonance value (0x0000-0x1fff) */
            float Resonance;
            uint16_t reserved0[3];
        };

        class Voice
        {

        protected:
            inline float NoteToHz(uint8_t note, float mod)
            {
                const float f0 = osc_notehzf(note);
                const float f1 = osc_notehzf(note + 1);

                const float f = clipmaxf(linintf(mod, f0, f1), k_note_max_hz);

                return f;
            }

        public:
            VoiceParams Params;
            virtual void IncrementEnvelope(uint32_t frames) = 0;
            virtual void Reset() = 0;
            virtual float Generate() = 0;
            virtual void UpdateOscParams(VoiceParams params) = 0;
            virtual void NoteOn() = 0;
            virtual void NoteOff() = 0;
            ~Voice()
            {
            }
        };
    } // namespace Dsp
} // namespace ByteFarm