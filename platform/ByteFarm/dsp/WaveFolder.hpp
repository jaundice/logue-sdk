#pragma once
#include "FxElement.hpp"
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

        template<size_t SAMPLERATE>
        class WaveFolder : public FxElement<WaveFolderParams, SAMPLERATE>
        {

            inline float Delta(float delta)
            {
                return fabs(delta);
                //return fasterpowf(fabs(delta), Params->Aggression);
            }

            inline float Folder(float s)
            {
                return s > this->Params->MaxLimit ? this->Params->MaxLimit - Delta(s - this->Params->MaxLimit) : this->Params->MinLimit + Delta(this->Params->MinLimit - s);
            }

            inline float Fold(float s)
            {
                if (s > this->Params->MinLimit && s < this->Params->MaxLimit)
                    return s;

                uint8_t i=0;
                do{
                    s = Folder(s);
                }while(i++ < 8 &&  this->Params->MinLimit || s > this->Params->MaxLimit);

                s = fx_softclipf(0.25f, s);

                return s;
            }

        public:
            WaveFolder(WaveFolderParams *params) : FxElement<WaveFolderParams, SAMPLERATE>(params)
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