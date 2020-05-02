#pragma once
#include "FxModule.hpp"
#include "FxElement.hpp"
#include "alienwah.hpp"
#include "common.h"

namespace ByteFarm
{
namespace Dsp
{
    


class AlienWahModule : public FxModule<1>
{

    static FxElementContainer<1>* GetModules()
    {

        AlienWahParams<3200> *p = new AlienWahParams<3200>(300.f, 1.2f, 20);
        FxElementBase* aw = (FxElementBase*)new AlienWah<3200>(p);
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