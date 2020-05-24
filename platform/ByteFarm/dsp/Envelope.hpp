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
            Loop = Release << 1
        };

        inline EnvelopeStage operator|(EnvelopeStage a, EnvelopeStage b)
        {
            return static_cast<EnvelopeStage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }
        inline EnvelopeStage operator&(EnvelopeStage a, EnvelopeStage b)
        {
            return static_cast<EnvelopeStage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
        }

        bool HasFlag(EnvelopeStage stages, EnvelopeStage test)
        {
            return (stages & test) == test;
        }

        enum EnvelopeSlope
        {
            Linear = 1,
            Exponential = Linear << 1
        };

        template <size_t SampleRate>
        class Envelope
        {
            const uint16_t FramesPerMs = SampleRate / 1000;
            EnvelopeStage Stages{Off};
            EnvelopeSlope Slope{EnvelopeSlope::Linear};
            EnvelopeStage CurrentStage{EnvelopeStage::Off};
            float Output = 0.0f;
            float SustainLevel = 0.75f;
            uint32_t DelayFrames = 0;
            uint32_t AttackFrames = 50;
            uint32_t HoldFrames = 1000;
            uint32_t DecayFrames = 200;
            uint32_t ReleaseFrames = 3000;
            uint32_t ElapsedFrames = 0;

            bool NoteDown;

            void TransitionStage()
            {
                switch (CurrentStage)
                {
                case Off:
                case Release:
                {
                    if (HasFlag(Stages, Loop))
                    {
                        CurrentStage = HasFlag(Stages, Delay) ? Delay : Attack;
                        return;
                    }

                    CurrentStage = Off;
                    return;
                }
                case Delay:{
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
                    CurrentStage = Release;
                    return;
                }
                }
            }

            void Calculate()
            {
                switch (CurrentStage)
                {
                case Delay:
                case Off:
                {
                    Output = 0.f;
                    return;
                }
                case Attack:
                {
                    Output = (float)ElapsedFrames / (float)AttackFrames; //just linear for now
                    return;
                }
                case Hold:
                {
                    Output = 1.f;
                    return;
                }
                case Decay:
                {
                    float lowLevel = HasFlag(Stages, Sustain) ? SustainLevel : 0.f;
                    Output = 1.f - ((1.f - lowLevel) * (float)ElapsedFrames / (float)DecayFrames);
                    return;
                }
                case Sustain:
                {
                    Output = SustainLevel;
                    return;
                }
                case Release:
                {
                    Output = SustainLevel - ((SustainLevel) * (float)ElapsedFrames / (float)ReleaseFrames);
                    return;
                }
                default:
                {
                    Output = 0.5f;
                }
                }
            }

        public:
            float CurrentValue()
            {
                //return 1.f;
                return Output;
            }

            void Increment()
            {
                switch (CurrentStage)
                {
                case Delay:
                {
                    if (ElapsedFrames > DelayFrames)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case Attack:
                {
                    if (ElapsedFrames > AttackFrames)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case Hold:
                {
                    if (ElapsedFrames > HoldFrames)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case Decay:
                {
                    if (ElapsedFrames > DecayFrames)
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
                    if (ElapsedFrames > ReleaseFrames)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                }

                Calculate();
                ElapsedFrames++;
            }

            void UpdateEnvelopeStage(EnvelopeStage stage, float milliseconds)
            {
                switch (stage)
                {
                case Delay:
                {
                    DelayFrames = milliseconds * FramesPerMs;
                    return;
                }
                case Attack:
                {
                    AttackFrames = milliseconds * FramesPerMs;
                    return;
                }
                case Hold:
                {
                    HoldFrames = milliseconds * FramesPerMs;
                    return;
                }
                case Decay:
                {
                    DecayFrames = milliseconds * FramesPerMs;
                    return;
                }
                case Release:
                {
                    ReleaseFrames = milliseconds * FramesPerMs;
                    return;
                }
                }
            }

            Envelope(EnvelopeStage envelopeStages, float delayMs, float attackMs, float holdMs, float decayMs, float releaseMs, float sustainLevel)
            {
                Stages = envelopeStages;
                SustainLevel = sustainLevel;
                UpdateEnvelopeStage(Delay, delayMs);
                UpdateEnvelopeStage(Attack, attackMs);
                UpdateEnvelopeStage(Hold, holdMs);
                UpdateEnvelopeStage(Decay, decayMs);
                UpdateEnvelopeStage(Release, releaseMs);
            }

            void Reset()
            {
                ElapsedFrames = 0;
                CurrentStage = Off;
                NoteDown = false;
            }

            void NoteOn()
            {
                NoteDown = true;
                switch (this->CurrentStage)
                {
                case Off:
                {
                    CurrentStage = HasFlag(Stages, Delay) ? Delay : Attack;
                    ElapsedFrames = 0;
                }
                }
            }

            void NoteOff()
            {
                NoteDown = false;
            }
        };
    } // namespace Dsp
} // namespace ByteFarm