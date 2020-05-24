#pragma once
#include "float_math.h"
namespace ByteFarm
{
    namespace Dsp
    {

        struct XY
        {
            float X;
            float Y;
        };

        class Geometry
        {
        public:
            static XY Spirograph(float largeRadius, float smallRadius, float offset, float scale, float angle)
            {
                float l = offset / smallRadius;
                float k = smallRadius / largeRadius;
                return Spirograph(k, l, scale, angle);
            }

            static XY Spirograph(float R, float r, float scale, float angle)
            {

                float oneMinusK = 1.f - R;
                float lk = r * R;

                return XY{
                    scale * (oneMinusK * fastercosf(angle)) + (lk * fastercosf(oneMinusK * angle / R)),

                    scale * (oneMinusK * fastersinf(angle)) - (lk * fastersinf(oneMinusK * angle / R))};
            }
        };
    } // namespace Dsp
} // namespace ByteFarm