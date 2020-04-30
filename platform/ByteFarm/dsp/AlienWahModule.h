#pragma once
#include <vector>
#include "FxModule.h"
#include "alienwah.h"
#include "common.h"


namespace ByteFarm{
    namespace Dsp{
        class AlienWahModule : public FxModule{

            static std::vector<FxElementBase*>* GetModules(){
                std::vector<FxElementBase*>  *mods = new std::vector<FxElementBase*>();
                //AlienWahParams<50> *p = new AlienWahParams<50>(300.f, 1.2f, 20);
                //AlienWah<50> *aw = new AlienWah<50>(p);

                //mods->push_back(aw);
                return mods;
            }

            public :
            AlienWahModule()
                : FxModule(GetModules()) { 
            }
        };
    }
}