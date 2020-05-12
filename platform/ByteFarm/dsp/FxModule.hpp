#pragma once
#include "FxElement.hpp"
#include "TypedArray.hpp"

namespace ByteFarm
{
    namespace Dsp
    {

        template <size_t NumElements>
        class FxModule
        {
        protected:
            TypedArray<FxElementBase *, NumElements> *Elements;

        public:
            FxModule(TypedArray<FxElementBase *, NumElements> *elements)
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

                    *(my++) = fx_softclipf(0.95f, /*  0.5f * s + 0.5f * */ result.Left);
                    *(my++) = fx_softclipf(0.95f, /* 0.5f * s2 + 0.5f * */ result.Right);
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