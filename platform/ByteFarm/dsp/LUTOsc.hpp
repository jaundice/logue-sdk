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

        public:
            LUTOsc(float * lut)
            {
                _table = lut;
                SetFreq(440.f);
                Reset();
            }

            void SetFreq(float freq)
            {
                freq = freq;
                stride = tl * freq;
            }

            void Reset()
            {
                index = 0.f;
            }

            float NextSample()
            {

                float idx = index;

                float idx2 = idx + 1.f;
                if (idx2 >= LUTLength)
                    idx2 -= LUTLength;


                //linear interpolation
                float f1 = *(_table + (int32_t)idx);
                float f2 = *(_table + (int32_t)idx2);

                float rat = idx - (int32_t)idx;

                idx += stride;
                if (idx >= LUTLength)
                    idx -= LUTLength;

                index = idx;
                return ((1.f - rat) * f1) + (rat * f2);
            }
        };
    } // namespace Dsp
} // namespace ByteFarm