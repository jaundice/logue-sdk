#pragma once
#include "FxModule.h"
#include "FxElement.h"
#include "alienwah.h"
#include "common.h"

namespace ByteFarm
{
namespace Dsp
{
    


class AlienWahModule : public FxModule<1>
{

    static FxElementContainer<1>* GetModules()
    {

        AlienWahParams<50> *p = new AlienWahParams<50>(300.f, 1.2f, 20);
        FxElementBase* aw = (FxElementBase*)new AlienWah<50>(p);
        FxElementContainer<1>* mods = new FxElementContainer<1>();
        mods->FxElements[0] = aw;

        return mods;
    }

public:
    AlienWahModule()
        : FxModule<1>(GetModules())
    {
    }
};
} // namespace Dsp
} // namespace ByteFarm