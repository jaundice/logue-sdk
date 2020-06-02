#pragma once
#include <inttypes.h>
#include "FxModule.hpp"
#include "Delay.hpp"
#include "SpeakAndSpellDegrader.hpp"
#include "alienwah.hpp"
#include "TypedArray.hpp"
#include "FxElement.hpp"
#include "userdelfx.h"
#include "common.h"
#include "FxElement.hpp"

#define WAHBUFFERSIZE 2048

namespace ByteFarm
{
    namespace Dsp
    {

        template <size_t DELAYBUFFERSIZE, size_t SAMPLERATE>
        static TypedArray<FxElementBase *, 1, uint8_t> *GetDelay()
        {

            SpeakAndSpellDegraderParams *speakandspellparams = new SpeakAndSpellDegraderParams();
            speakandspellparams->SetBits(12.f);
            speakandspellparams->Downsample = 23;

            FxElementBase *speakandspell = (FxElementBase *)new SpeakAndSpellDegrader<SAMPLERATE>(speakandspellparams);

            TypedArray<FxElementBase *, 1, uint8_t> *delayProcessors = new TypedArray<FxElementBase *, 1, uint8_t>();
            delayProcessors->Set(0, speakandspell);

            AlienWahParams<WAHBUFFERSIZE, SAMPLERATE> *alienwahparams = new AlienWahParams<WAHBUFFERSIZE, SAMPLERATE>(300.f, 0.3f, 200);

            FxElementBase *aw = (FxElementBase *)new AlienWah<WAHBUFFERSIZE, SAMPLERATE>(alienwahparams);

            TypedArray<FxElementBase *, 1, uint8_t> *feedbackProcessors = new TypedArray<FxElementBase *, 1, uint8_t>();
            feedbackProcessors->Set(0, aw);

            DelayParams *dp = new DelayParams(0.5f, 0.45f, 600);

            DelayBase<DELAYBUFFERSIZE, SAMPLERATE, 1, 1> *d = new DelayBase<DELAYBUFFERSIZE, SAMPLERATE, 1, 1>(dp, delayProcessors, feedbackProcessors);

            TypedArray<FxElementBase *, 1, uint8_t> *mods = new TypedArray<FxElementBase *, 1, uint8_t>();

            mods->Set(0, d);

            return mods;
        }

        template <size_t BufferSize, size_t SampleRate>
        class SuperDelayModule : public FxModule<1, SampleRate>
        {

        public:
            SuperDelayModule() : FxModule<1, SampleRate>(GetDelay<BufferSize, SampleRate>())
            {
            }

            inline virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
            {
                float val = fabs(q31_to_f32(value));
                DelayBase<BufferSize, SampleRate, 1, 1> *dly = (DelayBase<BufferSize, SampleRate, 1, 1> *)this->Elements->Get(0);

                switch (paramIndex)
                {
                case k_user_delfx_param_time:
                {

                    float totalms = ((float)BufferSize / (float)SampleRate) * 1000.f;
                    float bpm = fx_get_bpmf();
                    float msPer64 = 1000.f / (64.f * bpm);
                    uint32_t num64 = (uint32_t)(totalms / msPer64);
                    dly->Params->TimeMs = msPer64 * (float)(uint32_t)(val * num64);

                    break;
                }
                case k_user_delfx_param_shift_depth:
                {
                    dly->Params->Mix = val;
                    break;
                }
                case k_user_delfx_param_depth:
                {
                    dly->Params->Feedback = val * val;
                    break;
                }
                }

                float totalms = ((float)BufferSize / (float)SampleRate) * 1000.f;
                float bpm = fx_get_bpmf();
                float msPer64 = 1000.f / (64.f * bpm);
                uint32_t num64 = (uint32_t)(totalms / msPer64);
                uint32_t framesPer64 = msPer64 * SampleRate;

                uint32_t num32 = (uint32_t)(totalms / msPer64);
                dly->SetBufferBlockSize(framesPer64 * num64);
            }
        };

    } // namespace Dsp
} // namespace ByteFarm