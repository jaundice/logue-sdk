#include "FxModule.hpp"
#include "SpeakAndSpellDegrader.hpp"

namespace ByteFarm
{
    namespace Dsp
    {

        static FxElementContainer<1> *GetModules()
        {

            SpeakAndSpellDegraderParams *p = new SpeakAndSpellDegraderParams();
            FxElementBase *aw = (FxElementBase *)new SpeakAndSpellDegrader(p);
            FxElementContainer<1> *mods = new FxElementContainer<1>();
            mods->FxElements[0] = aw;

            return mods;
        }

        class SpeakAndSpellDegraderModule : public FxModule<1>
        {
        public:
            SpeakAndSpellDegraderModule() : FxModule<1>(GetModules())
            {
            }

            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
                float val = fabs(q31_to_f32(value));

                ByteFarm::Dsp::SpeakAndSpellDegrader *wf = static_cast<ByteFarm::Dsp::SpeakAndSpellDegrader *>(Elements->FxElements[0]);
                switch (paramIndex)
                {
                case 0:
                {
                    wf->Params->SetBits(val);
                    break;
                }
                case 1:
                {
                }
                }
            }
        };
    } // namespace Dsp
} // namespace ByteFarm
