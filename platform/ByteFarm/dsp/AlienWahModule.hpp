#pragma once
#include "FxModule.hpp"
#include "FxElement.hpp"
#include "AlienWah.hpp"
#include "common.h"
#include "TypedArray.hpp"

#define WAHBUFFERSIZE 3200

namespace ByteFarm
{
    namespace Dsp
    {

        class AlienWahModule : public FxModule<1>
        {

            static TypedArray<FxElementBase *, 1> *GetModules()
            {

                AlienWahParams<WAHBUFFERSIZE> *p = new AlienWahParams<WAHBUFFERSIZE>(300.f, 1.2f, 20);
                FxElementBase *aw = (FxElementBase *)new AlienWah<WAHBUFFERSIZE>(p);
                TypedArray<FxElementBase *, 1> *mods = new TypedArray<FxElementBase *, 1>();
                mods->Set(0, aw);

                return mods;
            }

        public:
            AlienWahModule()
                : FxModule<1>(GetModules())
            {
            }

            virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
            {
                float val = fabs(q31_to_f32(value));

                ByteFarm::Dsp::AlienWah<WAHBUFFERSIZE> *aw = static_cast<ByteFarm::Dsp::AlienWah<WAHBUFFERSIZE> *>(Elements->Get(0));

                switch (paramIndex)
                {
                case 0:
                {
                    aw->Params->lfo.setF0((fastexpf(val * val) - 1.f) * 8000.f, 1.f / SAMPLERATE);
                    break;
                }
                case 1:
                {
                    aw->Params->delay = (int32_t)fmaxf(1, (val)*WAHBUFFERSIZE) /* + WAHBUFFERSIZE/4.f */;
                    aw->Params->fb = 1.f + (fastexpf(val * val) - 1.f) * 30.f;
                    break;
                }
                case 2:

                    break;
                }
            }
        };
    } // namespace Dsp
} // namespace ByteFarm