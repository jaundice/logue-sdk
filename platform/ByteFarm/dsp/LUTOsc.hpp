#pragma once

namespace ByteFarm
{
    namespace Dsp
    {
        template <size_t LUTLength, size_t SampleRate>
        class LUTOsc
        {
            const float tl = (float)LUTLength / (float)SampleRate;
            float * _table;
            float freq;
            float stride;
            float index = 0.f;
            float startPhase = 0.f;
            float value = 0.f;

        public:
            LUTOsc(float * lut, float startPhase = 0.f)
            {
                _table = lut;
                this->startPhase = fabs(startPhase);
                SetFreq(440.f);
                Reset();
            }

            inline void SetFreq(float freq)
            {
                freq = freq;
                stride = tl * freq;
            }

            inline void Reset()
            {
                index = startPhase * (float)LUTLength;
            }

            inline float NextSample()
            {

                float idx = index;

                float idx2 = idx + 1.f;
                if (idx2 >= LUTLength)
                    idx2 -= LUTLength;
                else if(idx2 < 0 ){
                    idx2 += LUTLength;
                }


                //linear interpolation
                float f1 = *(_table + (int32_t)idx);
                float f2 = *(_table + (int32_t)idx2);

                float rat = idx - (int32_t)idx;

                idx += stride;
                if (idx >= LUTLength)
                    idx -= LUTLength;
                else if(idx < 0)
                    idx += LUTLength;

                index = idx;
                value =  ((1.f - rat) * f1) + (rat * f2);
                return value;
            }

            inline float Value(){
                return value;
            }
        };
    } // namespace Dsp
} // namespace ByteFarm