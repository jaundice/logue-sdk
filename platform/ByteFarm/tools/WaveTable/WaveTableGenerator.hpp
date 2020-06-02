#pragma once
#define _USE_MATH_DEFINES
#include <array>
#include <math.h>

namespace ByteFarm
{
    namespace Tools
    {

        typedef float (*Functor)(float angle);

        enum CommonWaveShapes{
            Sine =1,
            Tri = Sine <<1,
            Saw = Tri << 1,
            Sqr = Saw << 1
        };

        class CommonWaveTableFunctors
        {
        public:
            inline static float Sine(float angle)
            {
                return sin(angle);
            }

            template <uint8_t partials>
            inline static float Saw(float angle)
            {
                float res = 0;
                for (int i = 1; i < partials + 1; i++)
                {
                    res += Sine(i * angle) / i;
                }
                return res;
            }

            template <uint8_t partials>
            inline static float Tri(float angle)
            {
                float res = 0;
                for (int i = 1; i < partials + 1; i++)
                {
                    res += Sine(i * angle) / (i * i);
                }
                return res;
            }

            template <uint8_t partials>
            inline static float Square(float angle)
            {
                float res = 0;
                for (int i = 1; i < partials + 1; i += 2)
                {
                    res += Sine(i * angle) / i;
                }
                return res;
            }
        };

        class WaveTableGenerator
        {

        public:
            template <size_t arrSize>
            inline static std::array<float, arrSize> Generate(Functor functor)
            {
                const float w = 2 * M_PI / arrSize;
                std::array<float, arrSize> arr;

                Fill<arrSize>(arr.data(), functor);
                return arr;
            }

            template <size_t arrSize>
            inline static void Fill(float *arr, Functor functor)
            {
                const float w = 2 * M_PI / arrSize;

                for (int i = 0; i < arrSize; i++)
                {
                    float angle = w*i;
                    //arr[i] = i< arrSize/2 ?1.f :-1.f;//temp sqr wave
                    arr[i] = functor(angle);
                }
            }
        };
    } // namespace Tools
} // namespace ByteFarm