#pragma once
#include "TypedArray.hpp"

namespace ByteFarm
{
    namespace Dsp
    {

        template <size_t SIZE>
        class Coefficients
        {
        public:
            TypedArray<double, SIZE, uint8_t> a_coefficients;
            TypedArray<double, SIZE, uint8_t> b_coefficients;
            const uint8_t NumCoefficients;
            Coefficients(std::initializer_list<double> a, std::initializer_list<double> b, uint8_t numcoefficients) : a_coefficients{a},
                                                                                                                      b_coefficients{b},
                                                                                                                      NumCoefficients{numcoefficients}
            {
            }
            ~Coefficients()
            {
                //delete a_coefficients;
                //delete b_coefficients;
            }
        };

        __sdram Coefficients<6> *C12S = new Coefficients<6>{
            {0.036681502163648017, 0.2746317593794541, 0.56109896978791948, 0.769741833862266, 0.8922608180038789, 0.962094548378084},
            {0.13654762463195771, 0.42313861743656667, 0.6775400499741616, 0.839889624849638, 0.9315419599631839, 0.9878163707328971},
            6u};

        __sdram Coefficients<5> *C10S = new Coefficients<5>{
            {0.051457617441190984, 0.35978656070567017, 0.6725475931034693, 0.8590884928249939, 0.9540209867860787},
            {0.18621906251989334, 0.529951372847964, 0.7810257527489514, 0.9141815687605308, 0.985475023014907},
            5u};

        __sdram Coefficients<4> *C8S = new Coefficients<4>{
            {0.07711507983241622, 0.4820706250610472, 0.7968204713315797, 0.9412514277740471},
            {0.2659685265210946, 0.6651041532634957, 0.8841015085506159, 0.9820054141886075},
            4u};

        __sdram Coefficients<3> *C6S = new Coefficients<3>{
            {0.1271414136264853, 0.6528245886369117, 0.9176942834328115},
            {0.40056789819445626, 0.8204163891923343, 0.9763114515836773},
            3u};

        __sdram Coefficients<2> *C4S = new Coefficients<2>{
            {0.12073211751675449, 0.6632020224193995},
            {0.3903621872345006, 0.890786832653497},
            2u};

        __sdram Coefficients<1> *C2S = new Coefficients<1>{
            {0.23647102099689224},
            {0.7145421497126001},
            1u};

        __sdram Coefficients<6> *C12G = new Coefficients<6>{
            {0.01677466677723562, 0.13902148819717805, 0.3325011117394731, 0.53766105314488, 0.7214184024215805, 0.8821858402078155},
            {0.06501319274445962, 0.23094129990840923, 0.4364942348420355, /*0.06329609551399348*/ 0.6329609551399348, 0.80378086794111226, 0.9599687404800694},
            1u};

        __sdram Coefficients<5> *C10G = new Coefficients<5>{
            {0.02366831419883467, 0.18989476227180174, 0.43157318062118555, 0.6632020224193995, 0.860015542499582},
            {0.09056555904993387, 0.3078575723749043, 0.5516782402507934, 0.7652146863779808, 0.95247728378667541},
            5u};

        __sdram Coefficients<4> *C8G = new Coefficients<4>{
            {0.03583278843106211, 0.2720401433964576, 0.5720571972357003, 0.827124761997324},
            {0.1340901419430669, 0.4243248712718685, 0.7062921421386394, 0.9415030941737551},
            4u};

        __sdram Coefficients<3> *C6G = new Coefficients<3>{
            {0.06029739095712437, 0.4125907203610563, 0.7727156537429234},
            {0.21597144456092948, 0.6043586264658363, 0.9238861386532906},
            3u};

        __sdram Coefficients<2> *C4G = new Coefficients<2>{
            {0.07986642623635751, 0.5453536510711322},
            {0.28382934487410993, 0.8344118914807379},
            2u};

        __sdram Coefficients<1> *C2G = new Coefficients<1>{
            {0.23647102099689224},
            {0.7145421497126001},
            1u};

    } // namespace Dsp
} // namespace ByteFarm