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
            double a;
            AllPassFilterParams(double aa)
            {
                a = aa;
            }
        };

        class AllPassFilter : public FxElement<AllPassFilterParams>
        {
        public:
            AllPassFilter(AllPassFilterParams *params) : FxElement<AllPassFilterParams>(params)
            {
                Reset();
            };

            virtual void Reset() override
            {

                x0 = LRSample32F{0.f, 0.f};
                x1 = LRSample32F{0.f, 0.f};
                x2 = LRSample32F{0.f, 0.f};

                y0 = LRSample32F{0.f, 0.f};
                y1 = LRSample32F{0.f, 0.f};
                y2 = LRSample32F{0.f, 0.f};
            }

            virtual LRSample32F Process(LRSample32F input) override
            {
                x0 = input;

                //allpass filter 1
                const LRSample32F output{x2.Left + (float)((input.Left - y2.Left) * Params->a), x2.Right + (float)((input.Right - y2.Right) * Params->a)};

                y0 = output;

                return output;
            };

            virtual void Increment() override
            {
                //shuffle inputs
                x2 = x1;
                x1 = x0;

                //shuffle outputs
                y2 = y1;
                y1 = y0;
            };

        private:
            double a;

            LRSample32F x0;
            LRSample32F x1;
            LRSample32F x2;

            LRSample32F y0;
            LRSample32F y1;
            LRSample32F y2;
        };

        class AllPassFilterCascade
        {
        public:
            AllPassFilterCascade(double *coefficients, const uint8_t numCoefficients)
            {
                numfilters = numCoefficients;
                allpassfilters = (AllPassFilter **)malloc(sizeof(AllPassFilter *) * numCoefficients);
                for (uint8_t i = 0; i < numfilters; i++)
                {
                    double c = (coefficients)[i];
                    AllPassFilterParams *p = new AllPassFilterParams(c);
                    AllPassFilter *f = new AllPassFilter(p);
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

            virtual void Increment()
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

                int i = 0;

                for (uint8_t i = 0; i < numfilters; i++)
                {
                    output = allpassfilters[i]->Process(output);
                }

                return output;
            };

        private:
            AllPassFilter **allpassfilters;
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
            double *a_coefficients;
            double *b_coefficients;
            uint8_t NumCoefficients;

            static CascadedAllPassFilterParams *HalfBandFilter(HalfBandFilterOrder order, HalfBandFilterAngle angle)
            {

                double *a;
                double *b;
                uint8_t n;

                switch (angle)
                {
                case HalfBandFilterAngle::STEEP:
                {
                    switch (order)
                    {
                    case TWELVEPOLE:
                        a = (double *)C12S->a_coefficients.Ptr();
                        b = (double *)C12S->b_coefficients.Ptr();
                        n = C12S->NumCoefficients;
                        break;
                    case TENPOLE:
                        a = (double *)C10S->a_coefficients.Ptr();
                        b = (double *)C10S->b_coefficients.Ptr();
                        n = C10S->NumCoefficients;
                        break;
                    case EIGHTPOLE:
                        a = (double *)C8S->a_coefficients.Ptr();
                        b = (double *)C8S->b_coefficients.Ptr();
                        n = C8S->NumCoefficients;
                        break;
                    case SIXPOLE:
                        a = (double *)C6S->a_coefficients.Ptr();
                        b = (double *)C6S->b_coefficients.Ptr();
                        n = C6S->NumCoefficients;
                        break;
                    case FOURPOLE:
                        a = (double *)C4S->a_coefficients.Ptr();
                        b = (double *)C4S->b_coefficients.Ptr();
                        n = C4S->NumCoefficients;
                        break;
                    case TWOPOLE:
                        a = (double *)C2S->a_coefficients.Ptr();
                        b = (double *)C2S->b_coefficients.Ptr();
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
                        a = (double *)C12G->a_coefficients.Ptr();
                        b = (double *)C12G->b_coefficients.Ptr();
                        n = C12G->NumCoefficients;
                        break;
                    case TENPOLE:
                        a = (double *)C10G->a_coefficients.Ptr();
                        b = (double *)C10G->b_coefficients.Ptr();
                        n = C10G->NumCoefficients;
                        break;
                    case EIGHTPOLE:
                        a = (double *)C8G->a_coefficients.Ptr();
                        b = (double *)C8G->b_coefficients.Ptr();
                        n = C8G->NumCoefficients;
                        break;
                    case SIXPOLE:
                        a = (double *)C6G->a_coefficients.Ptr();
                        b = (double *)C6G->b_coefficients.Ptr();
                        n = C6G->NumCoefficients;
                        break;
                    case FOURPOLE:
                        a = (double *)C4G->a_coefficients.Ptr();
                        b = (double *)C4G->b_coefficients.Ptr();
                        n = C4G->NumCoefficients;
                        break;
                    case TWOPOLE:
                        a = (double *)C2G->a_coefficients.Ptr();
                        b = (double *)C2G->b_coefficients.Ptr();
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

        class HalfBandFilter : public FxElement<CascadedAllPassFilterParams>
        {
        public:
            HalfBandFilter(CascadedAllPassFilterParams *params) : FxElement<CascadedAllPassFilterParams>(params)
            {

                filter_a = new AllPassFilterCascade(Params->a_coefficients, Params->NumCoefficients);
                filter_b = new AllPassFilterCascade(Params->b_coefficients, Params->NumCoefficients);
                oldout = LRSample32F{0.f, 0.f};
            };
            ~HalfBandFilter()
            {
                delete filter_a;
                delete filter_b;
            };

            virtual void Increment()
            {
                filter_a->Increment();
                filter_b->Increment();
            }

            virtual LRSample32F Process(const LRSample32F input) override
            {
                const LRSample32F temp = filter_a->Process(input);
                const LRSample32F output{(temp.Left + oldout.Left) * 0.5f, (temp.Right + oldout.Right) * 0.5f};
                oldout = filter_b->Process(input);
                return output;
            }

        private:
            AllPassFilterCascade *filter_a;
            AllPassFilterCascade *filter_b;
            LRSample32F oldout;
        };

    } // namespace Dsp
} // namespace ByteFarm