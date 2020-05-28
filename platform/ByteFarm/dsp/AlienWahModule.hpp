#pragma once
#include "FxModule.hpp"
#include "FxElement.hpp"
#include "AlienWah.hpp"
#include "common.h"
#include "TypedArray.hpp"

#define WAHBUFFERSIZE 2048

namespace ByteFarm
{
    namespace Dsp
    {

        template <size_t SAMPLERATE>
        static TypedArray<FxElementBase *, 1, uint8_t> *GetModules()
        {

            AlienWahParams<WAHBUFFERSIZE, SAMPLERATE> *p = new AlienWahParams<WAHBUFFERSIZE, SAMPLERATE>(300.f, 1.2f, 20);
            FxElementBase *aw = (FxElementBase *)new AlienWah<WAHBUFFERSIZE, SAMPLERATE>(p);
            TypedArray<FxElementBase *, 1, uint8_t> *mods = new TypedArray<FxElementBase *, 1, uint8_t>();
            mods->Set(0, aw);

            return mods;
        };

        template <size_t SAMPLERATE>
        class AlienWahModule : public FxModule<1, SAMPLERATE>
        {

        public:
            AlienWahModule()
                : FxModule<1, SAMPLERATE>(GetModules<SAMPLERATE>())
            {
            }

            virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
            {
                float val = fabs(q31_to_f32(value));

                ByteFarm::Dsp::AlienWah<WAHBUFFERSIZE, SAMPLERATE> *aw = static_cast<ByteFarm::Dsp::AlienWah<WAHBUFFERSIZE, SAMPLERATE> *>(this->Elements->Get(0));

                switch (paramIndex)
                {
                case 0:
                {
                    aw->Params->lfo.setF0((fasterexpf(val * val) - 1.f) * 8000.f, 1.f / SAMPLERATE);
                    break;
                }
                case 1:
                {
                    aw->Params->delay = (int32_t)fmaxf(1, (val)*WAHBUFFERSIZE) /* + WAHBUFFERSIZE/4.f */;
                    aw->Params->fb = 1.f + (fasterexpf(val * val) - 1.f) * 30.f;
                    break;
                }
                case 2:

                    break;
                }
            }
        };
    } // namespace Dsp
} // namespace ByteFarm