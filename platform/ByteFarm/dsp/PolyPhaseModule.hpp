#pragma once
#include "FxElement.hpp"
#include "FxModule.hpp"
#include "AllPass.hpp"
#include "TypedArray.hpp"

namespace ByteFarm
{
	namespace Dsp
	{
		template <size_t SAMPLERATE>
		TypedArray<FxElementBase *, 1, uint8_t> *GetElements(HalfBandFilterOrder order, HalfBandFilterAngle angle)
		{

			TypedArray<FxElementBase *, 1, uint8_t> *elements = new TypedArray<FxElementBase *, 1, uint8_t>();

			CascadedAllPassFilterParams *p = CascadedAllPassFilterParams::HalfBandFilter(order, angle);
			HalfBandFilter<SAMPLERATE> *hbf = new HalfBandFilter<SAMPLERATE>(p);

			elements->Set(0, hbf);
			return elements;
		}
		template <size_t SAMPLERATE>
		HalfBandFilter<SAMPLERATE> *GetFilter(HalfBandFilterOrder order, HalfBandFilterAngle angle)
		{
			CascadedAllPassFilterParams *p = CascadedAllPassFilterParams::HalfBandFilter(order, angle);
			HalfBandFilter<SAMPLERATE> *hbf = new HalfBandFilter<SAMPLERATE>(p);
			return hbf;
		}

		template <size_t SAMPLERATE>
		TypedArray<FxElementBase *, 12, uint8_t> *GetAllElements()
		{

			TypedArray<FxElementBase *, 12, uint8_t> *elements = new TypedArray<FxElementBase *, 12, uint8_t>();

			elements->Set(0, GetFilter<SAMPLERATE>(HalfBandFilterOrder::TWOPOLE, HalfBandFilterAngle::GENTLE));
			elements->Set(1, GetFilter<SAMPLERATE>(HalfBandFilterOrder::FOURPOLE, HalfBandFilterAngle::GENTLE));
			elements->Set(2, GetFilter<SAMPLERATE>(HalfBandFilterOrder::SIXPOLE, HalfBandFilterAngle::GENTLE));
			elements->Set(3, GetFilter<SAMPLERATE>(HalfBandFilterOrder::EIGHTPOLE, HalfBandFilterAngle::GENTLE));
			elements->Set(4, GetFilter<SAMPLERATE>(HalfBandFilterOrder::TENPOLE, HalfBandFilterAngle::GENTLE));
			elements->Set(5, GetFilter<SAMPLERATE>(HalfBandFilterOrder::TWELVEPOLE, HalfBandFilterAngle::GENTLE));
			elements->Set(6, GetFilter<SAMPLERATE>(HalfBandFilterOrder::TWOPOLE, HalfBandFilterAngle::STEEP));
			elements->Set(7, GetFilter<SAMPLERATE>(HalfBandFilterOrder::FOURPOLE, HalfBandFilterAngle::STEEP));
			elements->Set(8, GetFilter<SAMPLERATE>(HalfBandFilterOrder::SIXPOLE, HalfBandFilterAngle::STEEP));
			elements->Set(9, GetFilter<SAMPLERATE>(HalfBandFilterOrder::EIGHTPOLE, HalfBandFilterAngle::STEEP));
			elements->Set(10, GetFilter<SAMPLERATE>(HalfBandFilterOrder::TENPOLE, HalfBandFilterAngle::STEEP));
			elements->Set(11, GetFilter<SAMPLERATE>(HalfBandFilterOrder::TWELVEPOLE, HalfBandFilterAngle::STEEP));
			return elements;
		}

		template <size_t SAMPLERATE>
		class PolyPhaseModule12S : public FxModule<1, SAMPLERATE>
		{
		public:
			PolyPhaseModule12S() : FxModule<1, SAMPLERATE>(GetElements<SAMPLERATE>(TWELVEPOLE, STEEP))
			{
			}
		};
		template <size_t SAMPLERATE>
		class PolyPhaseModule10S : public FxModule<1, SAMPLERATE>
		{
		public:
			PolyPhaseModule10S() : FxModule<1, SAMPLERATE>(GetElements<SAMPLERATE>(TENPOLE, STEEP))
			{
			}
		};
		template <size_t SAMPLERATE>
		class PolyPhaseModule8S : public FxModule<1, SAMPLERATE>
		{
		public:
			PolyPhaseModule8S() : FxModule<1, SAMPLERATE>(GetElements<SAMPLERATE>(EIGHTPOLE, STEEP))
			{
			}
		};
		template <size_t SAMPLERATE>
		class PolyPhaseModule6S : public FxModule<1, SAMPLERATE>
		{
		public:
			PolyPhaseModule6S() : FxModule<1, SAMPLERATE>(GetElements<SAMPLERATE>(SIXPOLE, STEEP))
			{
			}
		};
		template <size_t SAMPLERATE>
		class PolyPhaseModule4S : public FxModule<1, SAMPLERATE>
		{
		public:
			PolyPhaseModule4S() : FxModule<1, SAMPLERATE>(GetElements<SAMPLERATE>(FOURPOLE, STEEP))
			{
			}
		};
		template <size_t SAMPLERATE>
		class PolyPhaseModule2S : public FxModule<1, SAMPLERATE>
		{
		public:
			PolyPhaseModule2S() : FxModule<1, SAMPLERATE>(GetElements<SAMPLERATE>(TWOPOLE, STEEP))
			{
			}
		};
		template <size_t SAMPLERATE>
		class PolyPhaseModule12G : public FxModule<1, SAMPLERATE>
		{
		public:
			PolyPhaseModule12G() : FxModule<1, SAMPLERATE>(GetElements<SAMPLERATE>(TWELVEPOLE, GENTLE))
			{
			}
		};
		template <size_t SAMPLERATE>
		class PolyPhaseModule10G : public FxModule<1, SAMPLERATE>
		{
		public:
			PolyPhaseModule10G() : FxModule<1, SAMPLERATE>(GetElements<SAMPLERATE>(TENPOLE, GENTLE))
			{
			}
		};
		template <size_t SAMPLERATE>
		class PolyPhaseModule8G : public FxModule<1, SAMPLERATE>
		{
		public:
			PolyPhaseModule8G() : FxModule<1, SAMPLERATE>(GetElements<SAMPLERATE>(EIGHTPOLE, GENTLE))
			{
			}
		};
		template <size_t SAMPLERATE>
		class PolyPhaseModule6G : public FxModule<1, SAMPLERATE>
		{
		public:
			PolyPhaseModule6G() : FxModule<1, SAMPLERATE>(GetElements<SAMPLERATE>(SIXPOLE, GENTLE))
			{
			}
		};
		template <size_t SAMPLERATE>
		class PolyPhaseModule4G : public FxModule<1, SAMPLERATE>
		{
		public:
			PolyPhaseModule4G() : FxModule<1, SAMPLERATE>(GetElements<SAMPLERATE>(FOURPOLE, GENTLE))
			{
			}
		};
		template <size_t SAMPLERATE>
		class PolyPhaseModule2G : public FxModule<1, SAMPLERATE>
		{
		public:
			PolyPhaseModule2G() : FxModule<1, SAMPLERATE>(GetElements<SAMPLERATE>(TWOPOLE, GENTLE))
			{
			}
		};
		template <size_t SAMPLERATE>
		class PolyPhaseMultiModule : public FxModule<12, SAMPLERATE>
		{
			uint8_t elementIdx = 0;
			float mix = 0.f;

		public:
			PolyPhaseMultiModule() : FxModule<12, SAMPLERATE>(GetAllElements<SAMPLERATE>())
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
					this->Elements->Get(elementIdx)->Increment();
					result1 = this->Elements->Get(elementIdx)->Process(result1);

					//this->Elements->Get(elementIdx + 6)->Increment();
					//result2 = this->Elements->Get(elementIdx + 6)->Process(result2);

					*(my++) = fx_softclipf(0.025, /*  0.5f * s + 0.5f * */ result1.Left);  // + (1.f - mix) * s);
					*(my++) = fx_softclipf(0.025, /* 0.5f * s2 + 0.5f * */ result1.Right); // + (1.f - mix) * s2);
				}
			}

			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
				float val = q31_to_f32(value);
				switch (paramIndex)
				{
				case 0:
				{
					int8_t newIdx = value %12; //(int8_t)fabsf(val * 12.f);
					//newIdx+=6;
					if (newIdx > 11)
						newIdx = 11;
					if (this->elementIdx != newIdx)
					{
						this->elementIdx = newIdx;
						//this->Elements->Get(newIdx)->Reset();
						//this->Elements->Get(newIdx + 6)->Reset();
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
