#pragma once

namespace ByteFarm
{
    namespace Dsp
    {

        template <size_t SIZE>
        class Coefficients
        {
        public:
            float a_coefficients[SIZE];
            float b_coefficients[SIZE];
        };

        __sdram Coefficients<6> C12S {
            {0.036681502163648017f, 0.2746317593794541f, 0.56109896978791948f, 0.769741833862266f, 0.8922608180038789f, 0.962094548378084f},
            {0.13654762463195771f, 0.42313861743656667f, 0.6775400499741616f, 0.839889624849638f, 0.9315419599631839f, 0.9878163707328971f}
            };

        __sdram Coefficients<5> C10S {
            {0.051457617441190984f, 0.35978656070567017f, 0.6725475931034693f, 0.8590884928249939f, 0.9540209867860787f},
            {0.18621906251989334f, 0.529951372847964f, 0.7810257527489514f, 0.9141815687605308f, 0.985475023014907f}};

        __sdram Coefficients<4> C8S {
            {0.07711507983241622f, 0.4820706250610472f, 0.7968204713315797f, 0.9412514277740471f},
            {0.2659685265210946f, 0.6651041532634957f, 0.8841015085506159f, 0.9820054141886075f}};

        __sdram Coefficients<3> C6S {
            {0.1271414136264853f, 0.6528245886369117f, 0.9176942834328115f},
            {0.40056789819445626f, 0.8204163891923343f, 0.9763114515836773f}};

        __sdram Coefficients<2> C4S{
            {0.12073211751675449f, 0.6632020224193995f},
            {0.3903621872345006f, 0.890786832653497f}};

        __sdram Coefficients<1> C2S{
            {0.23647102099689224f},
            {0.7145421497126001f}};

        __sdram Coefficients<6> C12G{
            {0.01677466677723562f, 0.13902148819717805f, 0.3325011117394731f, 0.53766105314488f, 0.7214184024215805f, 0.8821858402078155f},
            {0.06501319274445962f, 0.23094129990840923f, 0.4364942348420355f, 0.06329609551399348 /*0.6329609551399348f*/, 0.80378086794111226f, 0.9599687404800694f}};

        __sdram Coefficients<5> C10G {
            {0.02366831419883467f, 0.18989476227180174f, 0.43157318062118555f, 0.6632020224193995f, 0.860015542499582f},
            {0.09056555904993387f, 0.3078575723749043f, 0.5516782402507934f, 0.7652146863779808f, 0.95247728378667541f}};

        __sdram Coefficients<4> C8G {
            {0.03583278843106211f, 0.2720401433964576f, 0.5720571972357003f, 0.827124761997324f},
            {0.1340901419430669f, 0.4243248712718685f, 0.7062921421386394f, 0.9415030941737551f}};

        __sdram Coefficients<3> C6G {
            {0.06029739095712437f, 0.4125907203610563f, 0.7727156537429234f},
            {0.21597144456092948f, 0.6043586264658363f, 0.9238861386532906f}};

        __sdram Coefficients<2> C4G {
            {0.07986642623635751f, 0.5453536510711322f},
            {0.28382934487410993f, 0.8344118914807379f}};

        __sdram Coefficients<1> C2G {
            {0.23647102099689224f},
            {0.7145421497126001f}};

    } // namespace Dsp
} // namespace ByteFarm