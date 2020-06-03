#pragma once
#include <inttypes.h>
namespace ByteFarm
{
    namespace Dsp
    {
        enum EnvelopeStage
        {
            Off,
            Delay = 1,
            Attack = Delay << 1,
            Hold = Attack << 1,
            Decay = Hold << 1,
            Sustain = Decay << 1,
            Release = Sustain << 1,
            Loop = Release << 1,
            Crossfade = Loop << 1
        };

        inline EnvelopeStage operator|(EnvelopeStage a, EnvelopeStage b)
        {
            return static_cast<EnvelopeStage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }
        inline EnvelopeStage operator&(EnvelopeStage a, EnvelopeStage b)
        {
            return static_cast<EnvelopeStage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
        }

        inline bool HasFlag(EnvelopeStage stages, EnvelopeStage test)
        {
            return (stages & test) == test;
        }

        enum EnvelopeSlope
        {
            Linear = 1,
            Exponential = Linear << 1,
            Log = Exponential << 1
        };

        template <size_t SampleRate>
        class Envelope
        {
            const static uint16_t FramesPerMs = SampleRate / 1000;
            EnvelopeStage Stages{Off};
            EnvelopeSlope Slope{EnvelopeSlope::Linear};
            EnvelopeStage CurrentStage{EnvelopeStage::Off};
            float Output = 0.0f;
            float SustainLevel = 0.75f;
            float Slop = 0.1f;
            float CrossfadeLevel = 0.f;

            float OneOverCrossfadeFrames = 4.f / (float)FramesPerMs;
            uint32_t CrossfadeElapsed = 0;

            uint32_t ModelDelayFrames = 0;
            uint32_t ModelAttackFrames = 50;
            uint32_t ModelHoldFrames = 1000;
            uint32_t ModelDecayFrames = 200;
            uint32_t ModelReleaseFrames = 3000;
            uint32_t ModelElapsedFrames = 0;

            float OneOverDelayFrames = 0;
            float OneOverAttackFrames = 50;
            float OneOverHoldFrames = 1000;
            float OneOverDecayFrames = 200;
            float OneOverReleaseFrames = 3000;
            uint32_t ElapsedFrames = 0;

            bool NoteDown;

            inline void TransitionStage()
            {
                switch (CurrentStage)
                {
                case Off:
                case Release:
                {
                    CrossfadeLevel = Output;
                    ApplySlop();
                    if (HasFlag(Stages, Loop))
                    {
                        CurrentStage = HasFlag(Stages, Delay) ? Delay : Attack;
                        return;
                    }

                    CurrentStage = Off;
                    return;
                }
                case Delay:
                {
                    CurrentStage = Attack;
                    return;
                }
                case Attack:
                {
                    CurrentStage = HasFlag(Stages, Hold) ? Hold : Decay;
                    return;
                }
                case Hold:
                {
                    CurrentStage = Decay;
                    return;
                }
                case Decay:
                {
                    if (HasFlag(Stages, Sustain))
                    {
                        CurrentStage = Sustain;
                    }
                    else if (HasFlag(Stages, Release))
                    {
                        CurrentStage = Release;
                    }
                    else if (HasFlag(Stages, Loop))
                    {
                        CurrentStage = HasFlag(Stages, Delay) ? Delay : Attack;
                    }
                    else
                    {
                        CurrentStage = Off;
                    }
                    return;
                }
                case Sustain:
                {
                    if (HasFlag(Stages, Release))
                    {
                        CurrentStage = Release;
                    }
                    else if (HasFlag(Stages, Loop))
                    {
                        CurrentStage = HasFlag(Stages, Delay) ? Delay : Attack;
                    }
                    else
                    {
                        CurrentStage = Off;
                    }
                    return;
                }
                }
            }

            inline void Calculate()
            {
                float crossfade = 0.f;

                if (HasFlag(Stages, Crossfade))
                {
                    float r = CrossfadeElapsed * OneOverCrossfadeFrames;
                    r = 1.f - r;
                    if (r > 0.f && r < 1.f)
                        crossfade = CrossfadeLevel * r;
                }

                switch (CurrentStage)
                {
                case Delay:
                case Off:
                {
                    Output = fmax(0.f, crossfade);
                    return;
                }
                case Attack:
                {
                    Output = fmax(((float)ElapsedFrames * (float)OneOverAttackFrames), crossfade); //just linear for now
                    return;
                }
                case Hold:
                {
                    Output = 1.f;
                    return;
                }
                case Decay:
                {
                    float lowLevel = HasFlag(Stages, Sustain) || HasFlag(Stages, Release) ? SustainLevel : 0.f;
                    Output = 1.f - ((1.f - lowLevel) * ((float)ElapsedFrames * (float)OneOverDecayFrames));
                    return;
                }
                case Sustain:
                {
                    Output = SustainLevel;
                    return;
                }
                case Release:
                {
                    Output = SustainLevel - ((SustainLevel) * ((float)ElapsedFrames * (float)OneOverReleaseFrames));
                    return;
                }
                }
            }

        public:
            inline float CurrentValue()
            {
                //return 1.f;
                return Output;
            }

            inline void Increment(uint32_t numFrames = 1)
            {
                CrossfadeElapsed++;

                switch (CurrentStage)
                {
                case Delay:
                {
                    if (ElapsedFrames * OneOverDelayFrames > 1.f)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case Attack:
                {
                    if (ElapsedFrames * OneOverAttackFrames > 1.f)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case Hold:
                {
                    if (ElapsedFrames * OneOverHoldFrames > 1.f)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case Decay:
                {
                    if (ElapsedFrames * OneOverDecayFrames > 1.f)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case Sustain:
                {
                    if (!NoteDown)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case Release:
                {
                    if (ElapsedFrames * OneOverReleaseFrames > 1.f)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                }

                ElapsedFrames += numFrames;
                Calculate();
            }

            inline void UpdateEnvelopeStage(EnvelopeStage stage, float milliseconds)
            {
                switch (stage)
                {
                case Delay:
                {
                    ModelDelayFrames = milliseconds * FramesPerMs;
                    return;
                }
                case Attack:
                {
                    ModelAttackFrames = milliseconds * FramesPerMs;
                    return;
                }
                case Hold:
                {
                    ModelHoldFrames = milliseconds * FramesPerMs;
                    return;
                }
                case Decay:
                {
                    ModelDecayFrames = milliseconds * FramesPerMs;
                    return;
                }
                case Release:
                {
                    ModelReleaseFrames = milliseconds * FramesPerMs;
                    return;
                }
                }
            }

            Envelope(EnvelopeStage envelopeStages,
                     float delayMs,
                     float attackMs,
                     float holdMs,
                     float decayMs,
                     float releaseMs,
                     float sustainLevel,
                     float slop)
            {
                Stages = envelopeStages;
                SustainLevel = sustainLevel;
                Slop = slop;
                UpdateEnvelopeStage(Delay, delayMs);
                UpdateEnvelopeStage(Attack, attackMs);
                UpdateEnvelopeStage(Hold, holdMs);
                UpdateEnvelopeStage(Decay, decayMs);
                UpdateEnvelopeStage(Release, releaseMs);

                ApplySlop();
            }

            inline void ApplySlop()
            {
                OneOverDelayFrames = 1.f / (float)(ModelDelayFrames + (uint32_t)(Slop * osc_white() * ModelDelayFrames));
                OneOverAttackFrames = 1.f / (float)(ModelAttackFrames + (uint32_t)(Slop * osc_white() * ModelAttackFrames));
                OneOverHoldFrames = 1.f / (float)(ModelHoldFrames + (uint32_t)(Slop * osc_white() * ModelHoldFrames));
                OneOverDecayFrames = 1.f / (float)(ModelDecayFrames + (uint32_t)(Slop * osc_white() * ModelDecayFrames));
                OneOverReleaseFrames = 1.f / (float)(ModelReleaseFrames + (uint32_t)(Slop * osc_white() * ModelReleaseFrames));
            }

            inline void Reset()
            {
                ElapsedFrames = 0;
                CurrentStage = Off;
                NoteDown = false;
            }

            inline void NoteOn()
            {
                CrossfadeLevel = Output;
                NoteDown = true;
                CrossfadeElapsed = 0;
                //switch (this->CurrentStage)
                //{
                //case Off:
                //{
                CurrentStage = HasFlag(Stages, Delay) ? Delay : Attack;
                ElapsedFrames = 0;
                //}
                //}
            }

            inline void SetSlop(float slop)
            {
                Slop = slop;
            }

            inline void NoteOff()
            {
                NoteDown = false;
            }
        };
    } // namespace Dsp
} // namespace ByteFarm