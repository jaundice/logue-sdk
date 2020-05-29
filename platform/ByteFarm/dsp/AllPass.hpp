// based on https://www.musicdsp.org/en/latest/Filters/39-polyphase-filters.html
#pragma once
#include "LRSample32F.hpp"
#include "FxElement.hpp"
#include "AllPassCoefficients.hpp"
#include <array>

namespace ByteFarm
{
    namespace Dsp
    {

        class AllPassFilterParams
        {
        public:
            float a;
            AllPassFilterParams(float aa)
            {
                a = aa;
            }
        };

        template <size_t SAMPLERATE>
        class AllPassFilter : public FxElement<AllPassFilterParams, SAMPLERATE>
        {
        public:
            AllPassFilter(AllPassFilterParams *params) : FxElement<AllPassFilterParams, SAMPLERATE>(params)
            {
                Reset();
            };

            virtual void Reset() override
            {

                *x0 = LRSample32F{0.f, 0.f};
                *x1 = LRSample32F{0.f, 0.f};
                *x2 = LRSample32F{0.f, 0.f};

                *y0 = LRSample32F{0.f, 0.f};
                *y1 = LRSample32F{0.f, 0.f};
                *y2 = LRSample32F{0.f, 0.f};
            }
            ~AllPassFilter()
            {
                delete x0;
                delete x1;
                delete x2;
                delete y0;
                delete y1;
                delete y2;
            }

            inline virtual LRSample32F Process(LRSample32F input) override
            {
                *x0 = input;

                //allpass filter 1
                const LRSample32F output{
                    x2->Left + (float)((input.Left - y2->Left) * this->Params->a),
                    x2->Right + (float)((input.Right - y2->Right) * this->Params->a)};

                *y0 = output;

                return output;
            };

            inline virtual void Increment() override
            {
                //shuffle inputs
                x2 = x1;
                x1 = x0;

                //shuffle outputs
                y2 = y1;
                y1 = y0;
            };

        private:
            float a;

            LRSample32F *x0 = new LRSample32F{0};
            LRSample32F *x1 = new LRSample32F{0};
            LRSample32F *x2 = new LRSample32F{0};

            LRSample32F *y0 = new LRSample32F{0};
            LRSample32F *y1 = new LRSample32F{0};
            LRSample32F *y2 = new LRSample32F{0};
        };
        template <size_t SAMPLERATE>
        class AllPassFilterCascade
        {
        public:
            AllPassFilterCascade(float *coefficients, const uint8_t numCoefficients)
            {
                numfilters = numCoefficients;
                allpassfilters = (AllPassFilter<SAMPLERATE> **)malloc(sizeof(AllPassFilter<SAMPLERATE> *) * numCoefficients);
                for (uint8_t i = 0; i < numfilters; i++)
                {
                    float c = (coefficients)[i];
                    AllPassFilterParams *p = new AllPassFilterParams(c);
                    AllPassFilter<SAMPLERATE> *f = new AllPassFilter<SAMPLERATE>(p);
                    allpassfilters[i] = f;
                }
            };
            ~AllPassFilterCascade()
            {
                for (uint8_t i = 0; i < numfilters; i++)
                {
                    delete allpassfilters[i];
                }
                delete allpassfilters;
            };

            inline virtual void Increment()
            {
                for (uint8_t i = 0; i < numfilters; i++)
                {
                    allpassfilters[i]->Increment();
                    /* code */
                }
            };

            LRSample32F Process(LRSample32F input)
            {
                LRSample32F output = input;
                for (uint8_t i = 0; i < numfilters; i++)
                {
                    output = allpassfilters[i]->Process(output);
                }
                return output;
            };

        private:
            AllPassFilter<SAMPLERATE> **allpassfilters;
            uint8_t numfilters;
        };

        enum HalfBandFilterOrder
        {
            TWELVEPOLE = 12,
            TENPOLE = 10,
            EIGHTPOLE = 8,
            SIXPOLE = 6,
            FOURPOLE = 4,
            TWOPOLE = 2
        };

        enum HalfBandFilterAngle
        {
            STEEP,
            GENTLE
        };

        class CascadedAllPassFilterParams
        {
        public:
            float *a_coefficients;
            float *b_coefficients;
            uint8_t NumCoefficients;

            static CascadedAllPassFilterParams *HalfBandFilter(HalfBandFilterOrder order, HalfBandFilterAngle angle)
            {

                float *a;
                float *b;
                uint8_t n;

                switch (angle)
                {
                case HalfBandFilterAngle::STEEP:
                {
                    switch (order)
                    {
                    case TWELVEPOLE:
                        a = (float *)C12S->a_coefficients.Ptr();
                        b = (float *)C12S->b_coefficients.Ptr();
                        n = C12S->NumCoefficients;
                        break;
                    case TENPOLE:
                        a = (float *)C10S->a_coefficients.Ptr();
                        b = (float *)C10S->b_coefficients.Ptr();
                        n = C10S->NumCoefficients;
                        break;
                    case EIGHTPOLE:
                        a = (float *)C8S->a_coefficients.Ptr();
                        b = (float *)C8S->b_coefficients.Ptr();
                        n = C8S->NumCoefficients;
                        break;
                    case SIXPOLE:
                        a = (float *)C6S->a_coefficients.Ptr();
                        b = (float *)C6S->b_coefficients.Ptr();
                        n = C6S->NumCoefficients;
                        break;
                    case FOURPOLE:
                        a = (float *)C4S->a_coefficients.Ptr();
                        b = (float *)C4S->b_coefficients.Ptr();
                        n = C4S->NumCoefficients;
                        break;
                    case TWOPOLE:
                        a = (float *)C2S->a_coefficients.Ptr();
                        b = (float *)C2S->b_coefficients.Ptr();
                        n = C2S->NumCoefficients;
                        break;
                    default:
                        a = {};
                        b = {};
                        break;
                    }
                    break;
                }
                case HalfBandFilterAngle::GENTLE:
                {
                    switch (order)
                    {
                    case TWELVEPOLE:
                        a = (float *)C12G->a_coefficients.Ptr();
                        b = (float *)C12G->b_coefficients.Ptr();
                        n = C12G->NumCoefficients;
                        break;
                    case TENPOLE:
                        a = (float *)C10G->a_coefficients.Ptr();
                        b = (float *)C10G->b_coefficients.Ptr();
                        n = C10G->NumCoefficients;
                        break;
                    case EIGHTPOLE:
                        a = (float *)C8G->a_coefficients.Ptr();
                        b = (float *)C8G->b_coefficients.Ptr();
                        n = C8G->NumCoefficients;
                        break;
                    case SIXPOLE:
                        a = (float *)C6G->a_coefficients.Ptr();
                        b = (float *)C6G->b_coefficients.Ptr();
                        n = C6G->NumCoefficients;
                        break;
                    case FOURPOLE:
                        a = (float *)C4G->a_coefficients.Ptr();
                        b = (float *)C4G->b_coefficients.Ptr();
                        n = C4G->NumCoefficients;
                        break;
                    case TWOPOLE:
                        a = (float *)C2G->a_coefficients.Ptr();
                        b = (float *)C2G->b_coefficients.Ptr();
                        n = C2G->NumCoefficients;
                        break;
                    default:
                        a = {};
                        b = {};
                        break;
                    }
                    break;
                }
                break;
                }
                return new CascadedAllPassFilterParams{
                    a,
                    b,
                    n};
            }; // namespace Dsp
        };

        template <size_t SAMPLERATE>
        class HalfBandFilter : public FxElement<CascadedAllPassFilterParams, SAMPLERATE>
        {
        public:
            HalfBandFilter(CascadedAllPassFilterParams *params) : FxElement<CascadedAllPassFilterParams, SAMPLERATE>(params)
            {

                filter_a = new AllPassFilterCascade<SAMPLERATE>(this->Params->a_coefficients, this->Params->NumCoefficients);
                filter_b = new AllPassFilterCascade<SAMPLERATE>(this->Params->b_coefficients, this->Params->NumCoefficients);
                oldout = LRSample32F{0.f, 0.f};
            };
            ~HalfBandFilter()
            {
                delete filter_a;
                delete filter_b;
            };

            inline virtual void Increment()
            {
                filter_a->Increment();
                filter_b->Increment();
            }

            inline virtual LRSample32F Process(const LRSample32F input) override
            {
                const LRSample32F temp = filter_a->Process(input);
                const LRSample32F output{(temp.Left + oldout.Left) * 0.5f, (temp.Right + oldout.Right) * 0.5f};
                oldout = filter_b->Process(input);
                return output;
            }

        private:
            AllPassFilterCascade<SAMPLERATE> *filter_a;
            AllPassFilterCascade<SAMPLERATE> *filter_b;
            LRSample32F oldout;
        };

    } // namespace Dsp
} // namespace ByteFarm