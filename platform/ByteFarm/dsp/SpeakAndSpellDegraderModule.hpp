#include "FxModule.hpp"
#include "SpeakAndSpellDegrader.hpp"
#include "TypedArray.hpp"
namespace ByteFarm
{
    namespace Dsp
    {

        static TypedArray<FxElementBase *, 1, uint8_t> *GetModules()
        {

            SpeakAndSpellDegraderParams *p = new SpeakAndSpellDegraderParams();
            FxElementBase *aw = (FxElementBase *)new SpeakAndSpellDegrader(p);
            TypedArray<FxElementBase *, 1, uint8_t> *mods = new TypedArray<FxElementBase *, 1, uint8_t>();
            mods->Set(0, aw);

            return mods;
        }

        class SpeakAndSpellDegraderModule : public FxModule<1>
        {
        public:
            SpeakAndSpellDegraderModule() : FxModule<1>(GetModules())
            {
            }

            virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
            {
                float val = fabs(q31_to_f32(value));

                ByteFarm::Dsp::SpeakAndSpellDegrader *wf = static_cast<ByteFarm::Dsp::SpeakAndSpellDegrader *>(Elements->Get(0));
                switch (paramIndex)
                {
                case 0:
                {
                    wf->Params->SetBits(16.f - (val * 15.f));
                    break;
                }
                case 1:
                {
                    wf->GetSandSParams()->Downsample = 1.f + (val * 63.f);
                    break;
                }
                }
            }
        };
    } // namespace Dsp
} // namespace ByteFarm
