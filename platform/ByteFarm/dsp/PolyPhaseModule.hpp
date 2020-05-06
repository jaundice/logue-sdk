#pragma once
#include "FxModule.hpp"
#include "AllPass.hpp"

namespace ByteFarm
{
    namespace Dsp
    {

        FxElementContainer<1>* GetElements(HalfBandFilterOrder order, HalfBandFilterAngle angle)
        {

            FxElementContainer<1> *elements = new FxElementContainer<1>();
            
            CascadedAllPassFilterParams *p =  CascadedAllPassFilterParams::HalfBandFilter(order, angle);
            HalfBandFilter *hbf =  new HalfBandFilter(p);
            
            elements->FxElements[0] =  hbf;
            return elements;
        }

        class PolyPhaseModule12S : public FxModule<1>
        {
        public:
            PolyPhaseModule12S() : FxModule<1>(GetElements(TWELVEPOLE, STEEP))
            {
            }
            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
            }
        };

        class PolyPhaseModule10S : public FxModule<1>
        {
        public:
            PolyPhaseModule10S() : FxModule<1>(GetElements(TENPOLE, STEEP))
            {
            }
            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
            }
        };

        class PolyPhaseModule8S : public FxModule<1>
        {
        public:
            PolyPhaseModule8S() : FxModule<1>(GetElements(EIGHTPOLE, STEEP))
            {
            }
            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
            }
        };

        class PolyPhaseModule6S : public FxModule<1>
        {
        public:
            PolyPhaseModule6S() : FxModule<1>(GetElements(SIXPOLE, STEEP))
            {
            }
            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
            }
        };

        class PolyPhaseModule4S : public FxModule<1>
        {
        public:
            PolyPhaseModule4S() : FxModule<1>(GetElements(FOURPOLE, STEEP))
            {
            }
            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
            }
        };

        class PolyPhaseModule2S : public FxModule<1>
        {
        public:
            PolyPhaseModule2S() : FxModule<1>(GetElements(TWOPOLE, STEEP))
            {
            }
            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
            }
        };

        class PolyPhaseModule12G : public FxModule<1>
        {
        public:
            PolyPhaseModule12G() : FxModule<1>(GetElements(TWELVEPOLE, GENTLE))
            {
            }
            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
            }
        };

        class PolyPhaseModule10G : public FxModule<1>
        {
        public:
            PolyPhaseModule10G() : FxModule<1>(GetElements(TENPOLE, GENTLE))
            {
            }
            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
            }
        };

        class PolyPhaseModule8G : public FxModule<1>
        {
        public:
            PolyPhaseModule8G() : FxModule<1>(GetElements(EIGHTPOLE, GENTLE))
            {
            }
            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
            }
        };

        class PolyPhaseModule6G : public FxModule<1>
        {
        public:
            PolyPhaseModule6G() : FxModule<1>(GetElements(SIXPOLE, GENTLE))
            {
            }
            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
            }
        };

        class PolyPhaseModule4G : public FxModule<1>
        {
        public:
            PolyPhaseModule4G() : FxModule<1>(GetElements(FOURPOLE, GENTLE))
            {
            }
            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
            }
        };

        class PolyPhaseModule2G : public FxModule<1>
        {
        public:
            PolyPhaseModule2G() : FxModule<1>(GetElements(TWOPOLE, GENTLE))
            {
            }
            virtual void UpdateParams(uint8_t paramIndex, int32_t value) const override
            {
            }
        };

    } // namespace Dsp
} // namespace ByteFarm
