#pragma once
#include <inttypes.h>
namespace ByteFarm
{
    namespace Dsp
    {
        template <uint16_t TableSize>
        class LUT
        {
        public:
            float Table[TableSize];
        };
    } // namespace Dsp
} // namespace ByteFarm