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
            return static_cast<EnvelopeStage>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
        }
        inline EnvelopeStage operator&(EnvelopeStage a, EnvelopeStage b)
        {
            return static_cast<EnvelopeStage>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
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
            EnvelopeStage Stages;
            EnvelopeSlope Slope;

            uint32_t ElapsedFrames;
            EnvelopeStage CurrentStage;
            float Output;
            float SustainLevel;
            uint32_t DelayFrames = 0;
            uint32_t DecayFrames = 0;
            uint32_t AttackFrames = 0;
            uint32_t HoldFrames = 0;
            uint32_t ReleaseFrames = 0;

            bool NoteDown;

            void TransitionStage()
            {
                switch (CurrentStage)
                {
                case EnvelopeStage::Release:
                {
                    if (Stages & EnvelopeStage::Loop == EnvelopeStage::Loop)
                    {
                        CurrentStage = Stages & EnvelopeStage::Delay == EnvelopeStage::Delay ? EnvelopeStage::Delay : EnvelopeStage::Attack;
                        return;
                    }

                    CurrentStage = EnvelopeStage::Off;
                    return;
                }
                case EnvelopeStage::Off:
                {
                    CurrentStage = Stages & EnvelopeStage::Delay == EnvelopeStage::Delay ? EnvelopeStage::Delay : EnvelopeStage::Attack;
                    return;
                }
                case EnvelopeStage::Attack:
                {
                    CurrentStage = Stages & EnvelopeStage::Hold == EnvelopeStage::Hold ? EnvelopeStage::Hold : EnvelopeStage::Decay;
                    return;
                }
                case EnvelopeStage::Hold:
                {
                    CurrentStage = EnvelopeStage::Decay;
                    return;
                }
                case EnvelopeStage::Decay:
                {
                    CurrentStage = Stages & EnvelopeStage::Sustain == EnvelopeStage::Sustain ? EnvelopeStage::Sustain : EnvelopeStage::Release;
                    return;
                }
                case EnvelopeStage::Sustain:
                {
                    CurrentStage = EnvelopeStage::Release;
                    return;
                }
                }
            }

            void Calculate()
            {
                switch (CurrentStage)
                {
                case EnvelopeStage::Delay:
                case EnvelopeStage::Off:
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
                    Output = 1.f - (1.f - SustainLevel) * (float)ElapsedFrames / (float)DecayFrames;
                    return;
                }
                case Sustain:
                {
                    Output = SustainLevel;
                    return;
                }
                case Release:
                {
                    Output = SustainLevel - (SustainLevel) * (float)ElapsedFrames / (float)DecayFrames;
                    return;
                }
                }
            }

        public:
            float CurrentValue()
            {
                return 1.f;
                //return Output;
            }

            void Increment()
            {
                switch (CurrentStage)
                {
                case EnvelopeStage::Delay:
                {
                    if (ElapsedFrames > DelayFrames)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case EnvelopeStage::Attack:
                {
                    if (ElapsedFrames > AttackFrames)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case EnvelopeStage::Hold:
                {
                    if (ElapsedFrames > HoldFrames)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case EnvelopeStage::Decay:
                {
                    if (ElapsedFrames > DecayFrames)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case EnvelopeStage::Sustain:
                {
                    if (!NoteDown)
                    {
                        ElapsedFrames = 0;
                        TransitionStage();
                    }
                    break;
                }
                case EnvelopeStage::Release:
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
                case EnvelopeStage::Delay:
                {
                    DelayFrames = milliseconds * FramesPerMs;
                    return;
                }
                case EnvelopeStage::Attack:
                {
                    AttackFrames = milliseconds * FramesPerMs;
                    return;
                }
                case EnvelopeStage::Hold:
                {
                    HoldFrames = milliseconds * FramesPerMs;
                    return;
                }
                case EnvelopeStage::Decay:
                {
                    DecayFrames = milliseconds * FramesPerMs;
                    return;
                }
                case EnvelopeStage::Release:
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
                CurrentStage = EnvelopeStage::Off;
                NoteDown = false;
            }

            void NoteOn()
            {
                NoteDown = true;
                switch(this->CurrentStage){
                    case Off:{
                        CurrentStage = Stages & Delay == Delay ? Delay: Attack;
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