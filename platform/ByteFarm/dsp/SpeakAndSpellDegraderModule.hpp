#include "FxModule.hpp"
#include "SpeakAndSpellDegrader.hpp"
#include "TypedArray.hpp"
namespace ByteFarm
{
    namespace Dsp
    {
        template <size_t SAMPLERATE>
        static TypedArray<FxElementBase *, 1, uint8_t> *GetModules()
        {

            SpeakAndSpellDegraderParams *p = new SpeakAndSpellDegraderParams();
            FxElementBase *aw = (FxElementBase *)new SpeakAndSpellDegrader<SAMPLERATE>(p);
            TypedArray<FxElementBase *, 1, uint8_t> *mods = new TypedArray<FxElementBase *, 1, uint8_t>();
            mods->Set(0, aw);

            return mods;
        }
        template <size_t SAMPLERATE>
        class SpeakAndSpellDegraderModule : public FxModule<1, SAMPLERATE>
        {
        public:
            SpeakAndSpellDegraderModule() : FxModule<1, SAMPLERATE>(GetModules<SAMPLERATE>())
            {
            }

            virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
            {
                float val = fabs(q31_to_f32(value));

                ByteFarm::Dsp::SpeakAndSpellDegrader<SAMPLERATE> *wf = static_cast<ByteFarm::Dsp::SpeakAndSpellDegrader<SAMPLERATE> *>(this->Elements->Get(0));
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
