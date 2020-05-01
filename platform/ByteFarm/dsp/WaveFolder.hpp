#pragma once
#include "FxElement.hpp"
#include "usermodfx.h"
#include "LRSample32F.hpp"
namespace ByteFarm
{
    namespace Dsp
    {
        class WaveFolderParams
        {
        public:
            float MaxLimit = 1.f;
            float MinLimit = -1.f;
            float Aggression = 1.f;
            float Gain = 1.f;
        };

        class WaveFolder : public FxElement<WaveFolderParams>
        {

            inline float Delta(float delta)
            {
                return fasterpowf(fabs(delta), Params->Aggression);
            }

            inline float Folder(float s)
            {
                return s > Params->MaxLimit ? Params->MaxLimit - Delta(s - Params->MaxLimit) : Params->MinLimit + Delta(Params->MinLimit - s);
            }

            inline float Fold(float s)
            {
                if (s > Params->MinLimit && s < Params->MaxLimit)
                    return s;

                if (s < Params->MinLimit || s > Params->MaxLimit)
                {
                    s = Folder(s);
                }
                if (s < Params->MinLimit || s > Params->MaxLimit)
                {
                    s = Folder(s);
                }
                if (s < Params->MinLimit || s > Params->MaxLimit)
                {
                    s = Folder(s);
                }
                if (s < Params->MinLimit || s > Params->MaxLimit)
                {
                    s = Folder(s);
                }
                if (s < Params->MinLimit || s > Params->MaxLimit)
                {
                    s = Folder(s);
                }
                if (s < Params->MinLimit || s > Params->MaxLimit)
                {
                    s = Folder(s);
                }
                if (s < Params->MinLimit || s > Params->MaxLimit)
                {
                    s = fx_softclipf(0, s); //fmax(MinLimit, fmin(MaxLimit, s)); //we are bouncing high and low too much so we just clip
                }

                return s;
            }

        public:
            WaveFolder(WaveFolderParams *params):FxElement<WaveFolderParams>(params)
            {

            }

            virtual void Increment() override
            {
            }

            virtual LRSample32F Process(LRSample32F sample) override
            {

                return LRSample32F{
                    Fold(sample.Left),
                    Fold(sample.Right)};
            }

            virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
            {
                float val = fabs(q31_to_f32(value));

                switch (paramIndex)
                {
                case 0:
                {
                    val = 1.f - val;
                    val = fmax(0.0001f, val);

                    Params->MaxLimit = val;
                    Params->MinLimit = -1.f * val;
                    Params->Gain = 1.f / val;
                    break;
                }
                case 1:
                {
                    Params->Aggression = 0.5f + (10 * (1.f - val));
                    break;
                }
                }
            };
        }; // namespace Dsp
    }      // namespace Dsp
} // namespace ByteFarm