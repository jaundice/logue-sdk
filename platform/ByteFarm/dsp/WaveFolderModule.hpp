#include "FxModule.hpp"
#include "WaveFolder.hpp"
#include "TypedArray.hpp"

namespace ByteFarm
{
    namespace Dsp
    {
        template <size_t SAMPLERATE>
        static TypedArray<FxElementBase *, 1, uint8_t> *GetModules()
        {

            WaveFolderParams *p = new WaveFolderParams();
            FxElementBase *aw = (FxElementBase *)new WaveFolder<SAMPLERATE>(p);
            TypedArray<FxElementBase *, 1, uint8_t> *mods = new TypedArray<FxElementBase *, 1, uint8_t>();
            mods->Set(0, aw);

            return mods;
        }

        template <size_t SAMPLERATE>
        class WaveFolderModule : public FxModule<1, SAMPLERATE>
        {
        public:
            WaveFolderModule() : FxModule<1, SAMPLERATE>(GetModules<SAMPLERATE>())
            {
            }

            inline virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
            {
                float val = fabs(q31_to_f32(value));
                val = 1.f - val;
                val = fmax(0.0001f, val);
                val *= val;

                ByteFarm::Dsp::WaveFolder<SAMPLERATE> *wf = static_cast<ByteFarm::Dsp::WaveFolder<SAMPLERATE> *>(this->Elements->Get(0));
                switch (paramIndex)
                {
                case 0:
                {

                    wf->Params->MaxLimit = val;
                    break;
                }
                case 1:
                {
                    wf->Params->MinLimit = -1.f * val;
                    break;
                }
                }
            }
        };
    } // namespace Dsp
} // namespace ByteFarm
