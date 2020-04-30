#pragma once
#include "FxElement.h"
#include <vector>

namespace ByteFarm
{
namespace Dsp
{

class FxModule
{

    std::vector<FxElementBase *> *Elements;

public:

    FxModule(std::vector<FxElementBase *> *elements)
    {
        Elements = elements;
    }

    void Process(const float *main_xn, float *main_yn, const float *sub_xn, float *sub_yn, uint32_t frames)
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

            for (uint16_t idx = 0; idx < Elements->size(); idx++)
            {
                Elements->at(idx)->Increment();
                result = Elements->at(idx)->Process(result);
            }

            *(my++) = fx_softclipf(0.95f, /*  0.5f * s + 0.5f * */ result.Left);
            *(my++) = fx_softclipf(0.95f, /* 0.5f * s2 + 0.5f * */ result.Right);
        }
    }

    virtual void UpdateParams(uint8_t fxElementIndex, uint8_t paramIndex, int32_t value)
    {
        this->Elements->at(fxElementIndex)->UpdateParams(paramIndex, value);
    }

    ~FxModule(){
        for(uint8_t i=0;i< this->Elements->size();i++){
            delete this->Elements->at(i);
        }
        delete Elements;
    }
};
} // namespace Dsp
} // namespace ByteFarm