// based on https://www.musicdsp.org/en/latest/Filters/39-polyphase-filters.html
#pragma once
#include "LRSample32F.hpp"
#include "FxElement.hpp"
#include <vector>
namespace ByteFarm
{
    namespace Dsp
    {

        class AllPassFilterParams
        {
        public:
            float a;
        };

        class AllPassFilter : public FxElement<AllPassFilterParams>
        {
        public:
            AllPassFilter(AllPassFilterParams *params) : FxElement<AllPassFilterParams>(params)
            {
                //a = coefficient;

                x0 = LRSample32F{0.f, 0.f};
                x1 = LRSample32F{0.f, 0.f};
                x2 = LRSample32F{0.f, 0.f};

                y0 = LRSample32F{0.f, 0.f};
                y1 = LRSample32F{0.f, 0.f};
                y2 = LRSample32F{0.f, 0.f};
            };
            virtual LRSample32F Process(LRSample32F input) override
            {
                x0 = input;

                //allpass filter 1
                const LRSample32F output{x2.Left + ((input.Left - y2.Left) * Params->a), x2.Right + ((input.Right - y2.Right) * Params->a)};

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
            float a;

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
            AllPassFilterCascade(const std::vector<float> coefficients)
            {
                allpassfilters = std::vector<AllPassFilter *>();

                for (int i = 0; i < coefficients.size(); i++)
                {
                    AllPassFilterParams *p = new AllPassFilterParams{coefficients.at(i)};
                    AllPassFilter *f = new AllPassFilter(p);
                    allpassfilters.push_back(f);
                }

                numfilters = allpassfilters.size();
            };
            ~AllPassFilterCascade()
            {
                for (size_t i = 0; i < allpassfilters.size(); i++)
                {
                    delete allpassfilters.at(i);
                }
                allpassfilters.clear();
            };

            virtual void Increment()
            {
                for (size_t i = 0; i < allpassfilters.size(); i++)
                {
                    allpassfilters.at(i)->Increment();
                    /* code */
                }
            }

            LRSample32F Process(LRSample32F input)
            {
                LRSample32F output = input;

                int i = 0;

                for (size_t i = 0; i < allpassfilters.size(); i++)
                {

                    output = allpassfilters.at(i)->Process(output);
                    /* code */
                }

                return output;
            };

        private:
            std::vector<AllPassFilter *> allpassfilters;
            int numfilters;
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

    /*  template <size_t SIZE>
        class Coefficients
        {
        public:
            float a_coefficients[SIZE];
            float b_coefficients[SIZE];
            size_t Size = SIZE;
            Coefficients(float[SIZE] a, float[SIZE] b)
            {
                a_coefficients = a;
                b_coefficients = b;
            }
        }; */

        class CascadedAllPassFilterParams
        {
        public:
            std::vector<float> a_coefficients;
            std::vector<float> b_coefficients;

            static CascadedAllPassFilterParams *HalfBandFilter(HalfBandFilterOrder order, HalfBandFilterAngle angle)
            {

                std::vector<float> a;
                std::vector<float> b;

                switch (angle)
                {
                case HalfBandFilterAngle::STEEP:
                {
                    switch (order)
                    {
                    case TWELVEPOLE:
                        a = {0.036681502163648017f, 0.2746317593794541f, 0.56109896978791948f, 0.769741833862266f, 0.8922608180038789f, 0.962094548378084f};
                        b = {0.13654762463195771f, 0.42313861743656667f, 0.6775400499741616f, 0.839889624849638f, 0.9315419599631839f, 0.9878163707328971f};
                        break;
                    case TENPOLE:
                        a = {0.051457617441190984f, 0.35978656070567017f, 0.6725475931034693f, 0.8590884928249939f, 0.9540209867860787f};
                        b = {0.18621906251989334f, 0.529951372847964f, 0.7810257527489514f, 0.9141815687605308f, 0.985475023014907f};
                        break;
                    case EIGHTPOLE:
                        a = {0.07711507983241622f, 0.4820706250610472f, 0.7968204713315797f, 0.9412514277740471f};
                        b = {0.2659685265210946f, 0.6651041532634957f, 0.8841015085506159f, 0.9820054141886075f};
                        break;
                    case SIXPOLE:
                        a = {0.1271414136264853f, 0.6528245886369117f, 0.9176942834328115f};
                        b = {0.40056789819445626f, 0.8204163891923343f, 0.9763114515836773f};
                        break;
                    case FOURPOLE:
                        a = {0.12073211751675449f, 0.6632020224193995f};
                        b = {0.3903621872345006f, 0.890786832653497f};
                        break;
                    case TWOPOLE:
                        a = {0.23647102099689224f};
                        b = {0.7145421497126001f};
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
                        a = {0.01677466677723562f, 0.13902148819717805f, 0.3325011117394731f, 0.53766105314488f, 0.7214184024215805f, 0.8821858402078155f};
                        b = {0.06501319274445962f, 0.23094129990840923f, 0.4364942348420355f, /*0.06329609551399348f*/ 0.6329609551399348f, 0.80378086794111226f, 0.9599687404800694f};
                        break;
                    case TENPOLE:
                        a = {0.02366831419883467f, 0.18989476227180174f, 0.43157318062118555f, 0.6632020224193995f, 0.860015542499582f};
                        b = {0.09056555904993387f, 0.3078575723749043f, 0.5516782402507934f, 0.7652146863779808f, 0.95247728378667541f};
                        break;
                    case EIGHTPOLE:
                        a = {0.03583278843106211f, 0.2720401433964576f, 0.5720571972357003f, 0.827124761997324f};
                        b = {0.1340901419430669f, 0.4243248712718685f, 0.7062921421386394f, 0.9415030941737551f};
                        break;
                    case SIXPOLE:
                        a = {0.06029739095712437f, 0.4125907203610563f, 0.7727156537429234f};
                        b = {0.21597144456092948f, 0.6043586264658363f, 0.9238861386532906f};
                        break;
                    case FOURPOLE:
                        a = {0.07986642623635751f, 0.5453536510711322f};
                        b = {0.28382934487410993f, 0.8344118914807379f};
                        break;
                    case TWOPOLE:
                        a = {0.23647102099689224f};
                        b = {0.7145421497126001f};
                        break;

                    default:
                        a = {};
                        b = {};
                        break;
                    }
                }
                break;
                }
                return new CascadedAllPassFilterParams{
                    a,
                    b};
            }; // namespace Dsp
        };

        class HalfBandFilter : public FxElement<CascadedAllPassFilterParams>
        {
        public:
            HalfBandFilter(CascadedAllPassFilterParams *params) : FxElement<CascadedAllPassFilterParams>(params)
            {

                filter_a = new AllPassFilterCascade(Params->a_coefficients);
                filter_b = new AllPassFilterCascade(Params->b_coefficients);
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