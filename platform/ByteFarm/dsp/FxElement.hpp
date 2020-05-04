#pragma once
#include "LRSample32F.hpp"
#include "common.h"

namespace ByteFarm
{
namespace Dsp
{

class FxElementBase
{
public:
    virtual void Increment() = 0;
    virtual LRSample32F Process(LRSample32F sample) = 0;
    virtual ~FxElementBase(){};
};

template <class TParams>
class FxElement : public FxElementBase
{
public:
    const int32_t SampleRate = SAMPLERATE;
    TParams *Params;

    virtual void Increment() override = 0;
    virtual LRSample32F Process(LRSample32F sample) override = 0;
    // virtual void UpdateParams(uint8_t paramIndex, int32_t value) override = 0;

    FxElement(TParams *params)
        : FxElementBase()
    {
        Params = params;
    }

    ~FxElement()
    {
        delete Params;
    }
};
} // namespace Dsp
} // namespace ByteFarm