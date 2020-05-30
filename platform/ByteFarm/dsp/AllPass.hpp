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
        class AllPassFilter 
        {
            float Coefficient;

        public:
            AllPassFilter(float coefficient) 
            {
                Coefficient = coefficient;
                Reset();
            };

            void Reset()
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

            inline LRSample32F Process(LRSample32F input)
            {
                *x0 = input;

                //allpass filter 1
                const LRSample32F output{
                    x2->Left + (float)((input.Left - y2->Left) * this->Coefficient),
                    x2->Right + (float)((input.Right - y2->Right) * this->Coefficient)};

                *y0 = output;

                //shuffle inputs
                x2 = x1;
                x1 = x0;

                //shuffle outputs
                y2 = y1;
                y1 = y0;

                return output;
            };

        private:
            
            LRSample32F *x0 = new LRSample32F{0};
            LRSample32F *x1 = new LRSample32F{0};
            LRSample32F *x2 = new LRSample32F{0};

            LRSample32F *y0 = new LRSample32F{0};
            LRSample32F *y1 = new LRSample32F{0};
            LRSample32F *y2 = new LRSample32F{0};
        };


        template <size_t SAMPLERATE, uint8_t NumFilters>
        class AllPassFilterCascade
        {

        public:
            AllPassFilterCascade(float *coefficients)
            {
                for (uint8_t i = 0; i < NumFilters; i++)
                {
                    float c = (coefficients)[i];
                    AllPassFilter *f = new AllPassFilter(c);
                    allpassfilters[i] = f;
                }
            };
            ~AllPassFilterCascade()
            {
                for (uint8_t i = 0; i < NumFilters; i++)
                {
                    delete allpassfilters[i];
                }
                //delete allpassfilters;
            };

            inline virtual void Increment()
            {
                // for (uint8_t i = 0; i < numfilters; i++)
                // {
                //     allpassfilters[i]->Increment();
                //     /* code */
                // }
            };

            LRSample32F Process(LRSample32F input)
            {
                LRSample32F output = input;
                for (uint8_t i = 0; i < NumFilters; i++)
                {
                    output = allpassfilters[i]->Process(output);
                }
                return output;
            };

        private:
            AllPassFilter *allpassfilters[NumFilters];
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

            static CascadedAllPassFilterParams *HalfBandFilter(HalfBandFilterOrder order, HalfBandFilterAngle angle)
            {

                float *a;
                float *b;

                switch (angle)
                {
                case HalfBandFilterAngle::STEEP:
                {
                    switch (order)
                    {
                    case TWELVEPOLE:
                        a = (float *)C12S.a_coefficients;
                        b = (float *)C12S.b_coefficients;
                        break;
                    case TENPOLE:
                        a = (float *)C10S.a_coefficients;
                        b = (float *)C10S.b_coefficients;
                        break;
                    case EIGHTPOLE:
                        a = (float *)C8S.a_coefficients;
                        b = (float *)C8S.b_coefficients;
                        break;
                    case SIXPOLE:
                        a = (float *)C6S.a_coefficients;
                        b = (float *)C6S.b_coefficients;
                        break;
                    case FOURPOLE:
                        a = (float *)C4S.a_coefficients;
                        b = (float *)C4S.b_coefficients;
                        break;
                    case TWOPOLE:
                        a = (float *)C2S.a_coefficients;
                        b = (float *)C2S.b_coefficients;
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
                        a = (float *)C12G.a_coefficients;
                        b = (float *)C12G.b_coefficients;
                        break;
                    case TENPOLE:
                        a = (float *)C10G.a_coefficients;
                        b = (float *)C10G.b_coefficients;
                        break;
                    case EIGHTPOLE:
                        a = (float *)C8G.a_coefficients;
                        b = (float *)C8G.b_coefficients;
                        break;
                    case SIXPOLE:
                        a = (float *)C6G.a_coefficients;
                        b = (float *)C6G.b_coefficients;
                        break;
                    case FOURPOLE:
                        a = (float *)C4G.a_coefficients;
                        b = (float *)C4G.b_coefficients;
                        break;
                    case TWOPOLE:
                        a = (float *)C2G.a_coefficients;
                        b = (float *)C2G.b_coefficients;
                        break;
                    }
                    break;
                }
                break;
                }
                return new CascadedAllPassFilterParams{
                    a,
                    b};
            }; // namespace Dsp
        };

        template <size_t SAMPLERATE, uint8_t NumFilters>
        class HalfBandFilter : public FxElement<CascadedAllPassFilterParams, SAMPLERATE>
        {
        public:
            HalfBandFilter(CascadedAllPassFilterParams *params) : FxElement<CascadedAllPassFilterParams, SAMPLERATE>(params)
            {

                filter_a = new AllPassFilterCascade<SAMPLERATE, NumFilters>(this->Params->a_coefficients);
                filter_b = new AllPassFilterCascade<SAMPLERATE, NumFilters>(this->Params->b_coefficients);
                oldout = LRSample32F{0.f, 0.f};
            };
            ~HalfBandFilter()
            {
                delete filter_a;
                delete filter_b;
            };

            inline virtual void Increment()
            {
                // filter_a->Increment();
                // filter_b->Increment();
            }

            inline virtual LRSample32F Process(const LRSample32F input) override
            {
                const LRSample32F temp = filter_a->Process(input);
                const LRSample32F output{(temp.Left + oldout.Left) * 0.5f, (temp.Right + oldout.Right) * 0.5f};
                oldout = filter_b->Process(input);
                return output;
            }

        private:
            AllPassFilterCascade<SAMPLERATE, NumFilters> *filter_a;
            AllPassFilterCascade<SAMPLERATE, NumFilters> *filter_b;
            LRSample32F oldout;
        };

    } // namespace Dsp
} // namespace ByteFarm