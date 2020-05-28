#pragma once
#include "FxElement.hpp"
#include "TypedArray.hpp"

namespace ByteFarm
{
    namespace Dsp
    {

        template <size_t NumElements, size_t SAMPLERATE>
        class FxModule
        {
        protected:
            TypedArray<FxElementBase *, NumElements, uint8_t> *Elements;

        public:
            FxModule(TypedArray<FxElementBase *, NumElements, uint8_t> *elements)
            {
                Elements = elements;
            }

            virtual void Process(const float *main_xn, float *main_yn, const float *sub_xn, float *sub_yn, uint32_t frames)
            {
                const float *mx = main_xn;
                float *__restrict my = main_yn;
                const float *my_e = my + 2 * frames;

                const float *sx = sub_xn;
                float *__restrict sy = sub_yn;

                // Loop through the samples
                for (; my != my_e;)
                {

                    // Pass sub through for prologue for now (L,R), you will need a separate filter at least if you want to process this properly
                    *(sy++) = *(sx++); // Copy *sy to *sx (Left channel)
                    *(sy++) = *(sx++); // Copy *sy to *sx (Right channel)

                    float s = (*mx++);
                    float s2 = (*mx++);

                    ByteFarm::Dsp::LRSample32F result{s, s2};

                    for (uint16_t idx = 0; idx < NumElements; idx++)
                    {
                        Elements->Get(idx)->Increment();
                        result = Elements->Get(idx)->Process(result);
                    }

                    *(my++) = fx_softclipf(0.05f, result.Left);
                    *(my++) = fx_softclipf(0.05f, result.Right);
                }
            }

            virtual void ProcessInPlace(float *main_xn, uint32_t frames)
            {
                float *mx = main_xn;
                float *mx_e = mx + 2 * frames;

                // Loop through the samples
                for (; mx != mx_e;)
                {
                    //float * pair = mx;

                    float * s = (mx++);
                    float * s2 = (mx++);

                    ByteFarm::Dsp::LRSample32F result{*s, *s2};

                    for (uint16_t idx = 0; idx < NumElements; idx++)
                    {
                        this->Elements->Get(idx)->Increment();
                        result = this->Elements->Get(idx)->Process(result);
                    }

                    *(s) = fx_softclipf(0.05f, result.Left);
                    *(s2) = fx_softclipf(0.05f, result.Right);
                }
            }

            virtual void UpdateParams(uint8_t paramIndex, int32_t value){};

            ~FxModule()
            {
                for (uint8_t i = 0; i < NumElements; i++)
                {
                    delete Elements;
                }
                delete Elements;
            }
        };
    } // namespace Dsp
} // namespace ByteFarm