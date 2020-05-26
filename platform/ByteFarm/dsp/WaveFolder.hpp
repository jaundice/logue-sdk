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
                return fabs(delta);
                //return fasterpowf(fabs(delta), Params->Aggression);
            }

            inline float Folder(float s)
            {
                return s > Params->MaxLimit ? Params->MaxLimit - Delta(s - Params->MaxLimit) : Params->MinLimit + Delta(Params->MinLimit - s);
            }

            inline float Fold(float s)
            {
                if (s > Params->MinLimit && s < Params->MaxLimit)
                    return s;

                uint8_t i=0;
                do{
                    s = Folder(s);
                }while(i++ < 8 &&  Params->MinLimit || s > Params->MaxLimit);

                s = fx_softclipf(0.25f, s);

                return s;
            }

        public:
            WaveFolder(WaveFolderParams *params) : FxElement<WaveFolderParams>(params)
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
        };
    } // namespace Dsp
} // namespace ByteFarm