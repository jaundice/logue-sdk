#pragma once
#include "FxElement.hpp"
#include "FxModule.hpp"
#include "AllPass.hpp"
#include "TypedArray.hpp"

namespace ByteFarm
{
	namespace Dsp
	{
		template <size_t SAMPLERATE, uint8_t NumFilters>
		TypedArray<FxElementBase *, 1, uint8_t> *GetElements(HalfBandFilterOrder order, HalfBandFilterAngle angle)
		{

			TypedArray<FxElementBase *, 1, uint8_t> *elements = new TypedArray<FxElementBase *, 1, uint8_t>();

			CascadedAllPassFilterParams *p = CascadedAllPassFilterParams::HalfBandFilter(order, angle);
			HalfBandFilter<SAMPLERATE, NumFilters> *hbf = new HalfBandFilter<SAMPLERATE, NumFilters>(p);

			elements->Set(0, hbf);
			return elements;
		}
		template <size_t SAMPLERATE, uint8_t NumFilters>
		HalfBandFilter<SAMPLERATE, NumFilters> *GetFilter(HalfBandFilterOrder order, HalfBandFilterAngle angle)
		{
			CascadedAllPassFilterParams *p = CascadedAllPassFilterParams::HalfBandFilter(order, angle);
			HalfBandFilter<SAMPLERATE, NumFilters> *hbf = new HalfBandFilter<SAMPLERATE, NumFilters>(p);
			return hbf;
		}

		template <size_t SAMPLERATE>
		TypedArray<FxElementBase *, 12, uint8_t> *GetAllElements()
		{

			TypedArray<FxElementBase *, 12, uint8_t> *elements = new TypedArray<FxElementBase *, 12, uint8_t>();

			elements->Set(0, GetFilter<SAMPLERATE, 2>(HalfBandFilterOrder::TWOPOLE, HalfBandFilterAngle::GENTLE));
			elements->Set(1, GetFilter<SAMPLERATE, 4>(HalfBandFilterOrder::FOURPOLE, HalfBandFilterAngle::GENTLE));
			elements->Set(2, GetFilter<SAMPLERATE, 6>(HalfBandFilterOrder::SIXPOLE, HalfBandFilterAngle::GENTLE));
			elements->Set(3, GetFilter<SAMPLERATE, 8>(HalfBandFilterOrder::EIGHTPOLE, HalfBandFilterAngle::GENTLE));
			elements->Set(4, GetFilter<SAMPLERATE, 10>(HalfBandFilterOrder::TENPOLE, HalfBandFilterAngle::GENTLE));
			elements->Set(5, GetFilter<SAMPLERATE, 12>(HalfBandFilterOrder::TWELVEPOLE, HalfBandFilterAngle::GENTLE));
			elements->Set(6, GetFilter<SAMPLERATE, 2>(HalfBandFilterOrder::TWOPOLE, HalfBandFilterAngle::STEEP));
			elements->Set(7, GetFilter<SAMPLERATE, 4>(HalfBandFilterOrder::FOURPOLE, HalfBandFilterAngle::STEEP));
			elements->Set(8, GetFilter<SAMPLERATE, 6>(HalfBandFilterOrder::SIXPOLE, HalfBandFilterAngle::STEEP));
			elements->Set(9, GetFilter<SAMPLERATE, 8>(HalfBandFilterOrder::EIGHTPOLE, HalfBandFilterAngle::STEEP));
			elements->Set(10, GetFilter<SAMPLERATE, 10>(HalfBandFilterOrder::TENPOLE, HalfBandFilterAngle::STEEP));
			elements->Set(11, GetFilter<SAMPLERATE, 12>(HalfBandFilterOrder::TWELVEPOLE, HalfBandFilterAngle::STEEP));
			return elements;
		}
		
		template <size_t SAMPLERATE>
		class PolyPhaseMultiModule : public FxModule<12, SAMPLERATE>
		{
			uint8_t elementIdx = 11;
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

					LRSample32F result= this->Elements->Get(elementIdx)->Process(LRSample32F{s, s2});
					
					
					*(my++) = fx_softclipf(0.025, result.Left);
					*(my++) = fx_softclipf(0.025, result.Right);
				}
			}

			virtual void UpdateParams(uint8_t paramIndex, int32_t value) override
			{
				float val = q31_to_f32(value);
				switch (paramIndex)
				{
				case 0:
				{
					uint8_t newIdx = (uint8_t)roundf((val * 6.f)+6.f);
					if (newIdx > 11)
						newIdx = 11;
					
						this->elementIdx = newIdx;
					break;
				}
				case 1:
				{
					this->mix = val;
					break;
				}
				}
			};
		};

	} // namespace Dsp
} // namespace ByteFarm
