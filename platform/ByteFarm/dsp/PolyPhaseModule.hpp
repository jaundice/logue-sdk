#pragma once
#include "FxModule.hpp"
#include "AllPass.hpp"

namespace ByteFarm
{
	namespace Dsp
	{

		FxElementContainer<1> *GetElements(HalfBandFilterOrder order, HalfBandFilterAngle angle)
		{

			FxElementContainer<1> *elements = new FxElementContainer<1>();

			CascadedAllPassFilterParams *p = CascadedAllPassFilterParams::HalfBandFilter(order, angle);
			HalfBandFilter *hbf = new HalfBandFilter(p);

			elements->FxElements[0] = hbf;
			return elements;
		}

		HalfBandFilter *GetFilter(HalfBandFilterOrder order, HalfBandFilterAngle angle)
		{
			CascadedAllPassFilterParams *p = CascadedAllPassFilterParams::HalfBandFilter(order, angle);
			HalfBandFilter *hbf = new HalfBandFilter(p);
			return hbf;
		}

		FxElementContainer<12> *GetAllElements()
		{

			FxElementContainer<12> *elements = new FxElementContainer<12>();

			elements->FxElements[0] = GetFilter(HalfBandFilterOrder::TWOPOLE, HalfBandFilterAngle::GENTLE);
			elements->FxElements[1] = GetFilter(HalfBandFilterOrder::FOURPOLE, HalfBandFilterAngle::GENTLE);
			elements->FxElements[2] = GetFilter(HalfBandFilterOrder::SIXPOLE, HalfBandFilterAngle::GENTLE);
			elements->FxElements[3] = GetFilter(HalfBandFilterOrder::EIGHTPOLE, HalfBandFilterAngle::GENTLE);
			elements->FxElements[4] = GetFilter(HalfBandFilterOrder::TENPOLE, HalfBandFilterAngle::GENTLE);
			elements->FxElements[5] = GetFilter(HalfBandFilterOrder::TWELVEPOLE, HalfBandFilterAngle::GENTLE);
			elements->FxElements[6] = GetFilter(HalfBandFilterOrder::TWOPOLE, HalfBandFilterAngle::STEEP);
			elements->FxElements[7] = GetFilter(HalfBandFilterOrder::FOURPOLE, HalfBandFilterAngle::STEEP);
			elements->FxElements[8] = GetFilter(HalfBandFilterOrder::SIXPOLE, HalfBandFilterAngle::STEEP);
			elements->FxElements[9] = GetFilter(HalfBandFilterOrder::EIGHTPOLE, HalfBandFilterAngle::STEEP);
			elements->FxElements[10] = GetFilter(HalfBandFilterOrder::TENPOLE, HalfBandFilterAngle::STEEP);
			elements->FxElements[11] = GetFilter(HalfBandFilterOrder::TWELVEPOLE, HalfBandFilterAngle::STEEP);
			return elements;
		}

		class PolyPhaseModule12S : public FxModule<1>
		{
		public:
			PolyPhaseModule12S() : FxModule<1>(GetElements(TWELVEPOLE, STEEP))
			{
			}
			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
			}
		};

		class PolyPhaseModule10S : public FxModule<1>
		{
		public:
			PolyPhaseModule10S() : FxModule<1>(GetElements(TENPOLE, STEEP))
			{
			}
			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
			}
		};

		class PolyPhaseModule8S : public FxModule<1>
		{
		public:
			PolyPhaseModule8S() : FxModule<1>(GetElements(EIGHTPOLE, STEEP))
			{
			}
			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
			}
		};

		class PolyPhaseModule6S : public FxModule<1>
		{
		public:
			PolyPhaseModule6S() : FxModule<1>(GetElements(SIXPOLE, STEEP))
			{
			}
			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
			}
		};

		class PolyPhaseModule4S : public FxModule<1>
		{
		public:
			PolyPhaseModule4S() : FxModule<1>(GetElements(FOURPOLE, STEEP))
			{
			}
			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
			}
		};

		class PolyPhaseModule2S : public FxModule<1>
		{
		public:
			PolyPhaseModule2S() : FxModule<1>(GetElements(TWOPOLE, STEEP))
			{
			}
			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
			}
		};

		class PolyPhaseModule12G : public FxModule<1>
		{
		public:
			PolyPhaseModule12G() : FxModule<1>(GetElements(TWELVEPOLE, GENTLE))
			{
			}
			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
			}
		};

		class PolyPhaseModule10G : public FxModule<1>
		{
		public:
			PolyPhaseModule10G() : FxModule<1>(GetElements(TENPOLE, GENTLE))
			{
			}
			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
			}
		};

		class PolyPhaseModule8G : public FxModule<1>
		{
		public:
			PolyPhaseModule8G() : FxModule<1>(GetElements(EIGHTPOLE, GENTLE))
			{
			}
			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
			}
		};

		class PolyPhaseModule6G : public FxModule<1>
		{
		public:
			PolyPhaseModule6G() : FxModule<1>(GetElements(SIXPOLE, GENTLE))
			{
			}
			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
			}
		};

		class PolyPhaseModule4G : public FxModule<1>
		{
		public:
			PolyPhaseModule4G() : FxModule<1>(GetElements(FOURPOLE, GENTLE))
			{
			}
			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
			}
		};

		class PolyPhaseModule2G : public FxModule<1>
		{
		public:
			PolyPhaseModule2G() : FxModule<1>(GetElements(TWOPOLE, GENTLE))
			{
			}
			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
			}
		};

		class PolyPhaseMultiModule : public FxModule<12>
		{
			uint8_t elementIdx = 0;
			float mix = 0.f;

		public:
			PolyPhaseMultiModule() : FxModule<12>(GetAllElements())
			{
			}

			virtual void Process(const float *main_xn, float *main_yn, const float *sub_xn, float *sub_yn, uint32_t frames) override
			{
				const float *mx = main_xn;
				float *__restrict my = main_yn;
				const float *my_e = my + 2 * frames;

				const float *sx = sub_xn;
				float *__restrict sy = sub_yn;

				// Loop through the samples
				for (; my != my_e;)
				{

					// Pass sub through for prologue for now (L,R), you will need a separate filter at least if you want to process this properly
					*(sy++) = *(sx++); // Copy *sy to *sx (Left channel)
					*(sy++) = *(sx++); // Copy *sy to *sx (Right channel)

					float s = (*mx++);
					float s2 = (*mx++);

					ByteFarm::Dsp::LRSample32F result1{s, s2};
					ByteFarm::Dsp::LRSample32F result2{s, s2};

					//we only want to use the selected fxelement
					Elements->FxElements[elementIdx]->Increment();
					result1 = Elements->FxElements[elementIdx]->Process(result1);

					Elements->FxElements[elementIdx + 6]->Increment();
					result2 = Elements->FxElements[elementIdx + 6]->Process(result2);

					*(my++) = fx_softclipf(0.95f, /*  0.5f * s + 0.5f * */ mix * result1.Left + (1.f - mix) * result2.Left);
					*(my++) = fx_softclipf(0.95f, /* 0.5f * s2 + 0.5f * */ mix * result1.Right + (1.f - mix) * result2.Right);
				}
			}

			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
				float val = fabs(q31_to_f32(value));
				switch (paramIndex)
				{
				case 0:
				{
					uint8_t newIdx = (uint8_t)(val * 5.f);
					if(this->elementIdx != newIdx)
					{
						this->elementIdx = newIdx ;
						Elements->FxElements[newIdx]->Reset();
						Elements->FxElements[newIdx+6]->Reset();	
					}

				}
				case 1:
				{
					this->mix = val;
				}
				}
			};
		};

	} // namespace Dsp
} // namespace ByteFarm
