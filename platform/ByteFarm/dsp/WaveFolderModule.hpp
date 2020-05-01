#include "FxModule.hpp"
#include "WaveFolder.hpp"

namespace ByteFarm
{
    namespace Dsp
    {

        static FxElementContainer<1> *GetModules()
        {

            WaveFolderParams *p = new WaveFolderParams();
            FxElementBase *aw = (FxElementBase *)new WaveFolder(p);
            FxElementContainer<1> *mods = new FxElementContainer<1>();
            mods->FxElements[0] = aw;

            return mods;
        }

        class WaveFolderModule : public FxModule<1>
        {
            public:
            WaveFolderModule() : FxModule<1>(GetModules())
            {
            }
        };
    } // namespace Dsp
} // namespace ByteFarm
