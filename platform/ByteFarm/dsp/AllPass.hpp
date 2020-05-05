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
        const LRSample32F output{x2.Left + ((input.Left - y2.Left) * this->Params.a), x2.Right + ((input.Right - y2.Right) * this->Params.a)};

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
    AllPassFilterCascade(const float *coefficients, int N)
    {
        allpassfilter = std::vector<AllPassFilter *>();

        for (int i = 0; i < N; i++)
        {
            AllPassFilterParams *p = new AllPassFilterParams{coefficients[i]};
            AllPassFilter *f = new AllPassFilter(p);
            allpassfilter.push_back(f);
        }

        numfilters = N;
    };
    ~AllPassFilterCascade()
    {
        for (size_t i = 0; i < allpassfilter.size(); i++)
        {
            delete allpassfilter.at(i);
        }
        allpassfilter.clear();
    };

    LRSample32F Process(LRSample32F input)
    {
        LRSample32F output = input;

        int i = 0;

        do
        {
            output = allpassfilter.at(i)->Process(output);
            i++;

        } while (i < numfilters);

        return output;
    };

private:
    std::vector<AllPassFilter *> allpassfilter;
    int numfilters;
};

class HalfBandFilter
{
public:
    HalfBandFilter(const int order, const bool steep)
    {
        if (steep == true)
        {
            if (order == 12) //rejection=104dB, transition band=0.01
            {
                float a_coefficients[6] =
                    {0.036681502163648017f, 0.2746317593794541f, 0.56109896978791948f, 0.769741833862266f, 0.8922608180038789f, 0.962094548378084f};

                float b_coefficients[6] =
                    {0.13654762463195771f, 0.42313861743656667f, 0.6775400499741616f, 0.839889624849638f, 0.9315419599631839f, 0.9878163707328971f};

                filter_a = new AllPassFilterCascade(a_coefficients, 6);
                filter_b = new AllPassFilterCascade(b_coefficients, 6);
            }
            else if (order == 10) //rejection=86dB, transition band=0.01
            {
                float a_coefficients[5] =
                    {0.051457617441190984f, 0.35978656070567017f, 0.6725475931034693f, 0.8590884928249939f, 0.9540209867860787f};

                float b_coefficients[5] =
                    {0.18621906251989334f, 0.529951372847964f, 0.7810257527489514f, 0.9141815687605308f, 0.985475023014907f};

                filter_a = new AllPassFilterCascade(a_coefficients, 5);
                filter_b = new AllPassFilterCascade(b_coefficients, 5);
            }
            else if (order == 8) //rejection=69dB, transition band=0.01
            {
                float a_coefficients[4] =
                    {0.07711507983241622f, 0.4820706250610472f, 0.7968204713315797f, 0.9412514277740471f};

                float b_coefficients[4] =
                    {0.2659685265210946f, 0.6651041532634957f, 0.8841015085506159f, 0.9820054141886075f};

                filter_a = new AllPassFilterCascade(a_coefficients, 4);
                filter_b = new AllPassFilterCascade(b_coefficients, 4);
            }
            else if (order == 6) //rejection=51dB, transition band=0.01
            {
                float a_coefficients[3] =
                    {0.1271414136264853f, 0.6528245886369117f, 0.9176942834328115f};

                float b_coefficients[3] =
                    {0.40056789819445626f, 0.8204163891923343f, 0.9763114515836773f};

                filter_a = new AllPassFilterCascade(a_coefficients, 3);
                filter_b = new AllPassFilterCascade(b_coefficients, 3);
            }
            else if (order == 4) //rejection=53dB,transition band=0.05
            {
                float a_coefficients[2] =
                    {0.12073211751675449f, 0.6632020224193995f};

                float b_coefficients[2] =
                    {0.3903621872345006f, 0.890786832653497f};

                filter_a = new AllPassFilterCascade(a_coefficients, 2);
                filter_b = new AllPassFilterCascade(b_coefficients, 2);
            }

            else //order=2, rejection=36dB, transition band=0.1
            {
                float a_coefficients = 0.23647102099689224f;
                float b_coefficients = 0.7145421497126001f;

                filter_a = new AllPassFilterCascade(&a_coefficients, 1);
                filter_b = new AllPassFilterCascade(&b_coefficients, 1);
            }
        }
        else //softer slopes, more attenuation and less stopband ripple
        {
            if (order == 12) //rejection=150dB, transition band=0.05
            {
                float a_coefficients[6] =
                    {0.01677466677723562f, 0.13902148819717805f, 0.3325011117394731f, 0.53766105314488f, 0.7214184024215805f, 0.8821858402078155f};

                float b_coefficients[6] =
                    {0.06501319274445962f, 0.23094129990840923f, 0.4364942348420355f, /*0.06329609551399348f*/  0.6329609551399348f, 0.80378086794111226f, 0.9599687404800694f};

                filter_a = new AllPassFilterCascade(a_coefficients, 6);
                filter_b = new AllPassFilterCascade(b_coefficients, 6);
            }
            else if (order == 10) //rejection=133dB, transition band=0.05
            {
                float a_coefficients[5] =
                    {0.02366831419883467f, 0.18989476227180174f, 0.43157318062118555f, 0.6632020224193995f, 0.860015542499582f};

                float b_coefficients[5] =
                    {0.09056555904993387f, 0.3078575723749043f, 0.5516782402507934f, 0.7652146863779808f, 0.95247728378667541f};

                filter_a = new AllPassFilterCascade(a_coefficients, 5);
                filter_b = new AllPassFilterCascade(b_coefficients, 5);
            }
            else if (order == 8) //rejection=106dB, transition band=0.05
            {
                float a_coefficients[4] =
                    {0.03583278843106211f, 0.2720401433964576f, 0.5720571972357003f, 0.827124761997324f};

                float b_coefficients[4] =
                    {0.1340901419430669f, 0.4243248712718685f, 0.7062921421386394f, 0.9415030941737551f};

                filter_a = new AllPassFilterCascade(a_coefficients, 4);
                filter_b = new AllPassFilterCascade(b_coefficients, 4);
            }
            else if (order == 6) //rejection=80dB, transition band=0.05
            {
                float a_coefficients[3] =
                    {0.06029739095712437f, 0.4125907203610563f, 0.7727156537429234f};

                float b_coefficients[3] =
                    {0.21597144456092948f, 0.6043586264658363f, 0.9238861386532906f};

                filter_a = new AllPassFilterCascade(a_coefficients, 3);
                filter_b = new AllPassFilterCascade(b_coefficients, 3);
            }
            else if (order == 4) //rejection=70dB,transition band=0.1
            {
                float a_coefficients[2] =
                    {0.07986642623635751f, 0.5453536510711322f};

                float b_coefficients[2] =
                    {0.28382934487410993f, 0.8344118914807379f};

                filter_a = new AllPassFilterCascade(a_coefficients, 2);
                filter_b = new AllPassFilterCascade(b_coefficients, 2);
            }

            else //order=2, rejection=36dB, transition band=0.1
            {
                float a_coefficients = 0.23647102099689224f;
                float b_coefficients = 0.7145421497126001f;

                filter_a = new AllPassFilterCascade(&a_coefficients, 1);
                filter_b = new AllPassFilterCascade(&b_coefficients, 1);
            }
        }

        oldout = LRSample32F{0.f, 0.f};
    };
    ~HalfBandFilter()
    {
        delete filter_a;
        delete filter_b;
    };

    LRSample32F Process(const LRSample32F input)
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