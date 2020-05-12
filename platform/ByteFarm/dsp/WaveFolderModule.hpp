#include "FxModule.hpp"
#include "WaveFolder.hpp"
#include "TypedArray.hpp"

namespace ByteFarm
{
    namespace Dsp
    {

        static TypedArray<FxElementBase *, 1> *GetModules()
        {

            WaveFolderParams *p = new WaveFolderParams();
            FxElementBase *aw = (FxElementBase *)new WaveFolder(p);
            TypedArray<FxElementBase *, 1> *mods = new TypedArray<FxElementBase *, 1>();
            mods->Set(0, aw);

            return mods;
        }

        class WaveFolderModule : public FxModule<1>
        {
        public:
            WaveFolderModule() : FxModule<1>(GetModules())
            {
            }

            virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
            {
                float val = fabs(q31_to_f32(value));

                ByteFarm::Dsp::WaveFolder *wf = static_cast<ByteFarm::Dsp::WaveFolder *>(Elements->Get(0));
                switch (paramIndex)
                {
                case 0:
                {
                    val = 1.f - val;
                    val = fmax(0.0001f, val);

                    wf->Params->MaxLimit = val;
                    wf->Params->MinLimit = -1.f * val;
                    wf->Params->Gain = 1.f / val;
                    break;
                }
                case 1:
                {
                    wf->Params->Aggression = 0.5f + (10 * (1.f - val));
                    break;
                }
                }
            }
        };
    } // namespace Dsp
} // namespace ByteFarm
