// MultiModeFilter.h
#pragma once
#include "LRSample32F.hpp"
#include "FxElement.hpp"

namespace ByteFarm
{
	namespace Dsp
	{

		enum FilterMode
		{
			FILTER_MODE_LOWPASS = 0,
			FILTER_MODE_HIGHPASS,
			FILTER_MODE_BANDPASS,
			kNumFilterModes
		};

		enum FilterPoles
		{
			FILTER_POLES_TWO = 0,
			FILTER_POLES_FOUR
		};

		class MultiModeFilterParams
		{
		public:
			float Cutoff;
			float Resonance;
			FilterMode Mode;
			FilterPoles Poles;
			float FeedbackAmount;

			MultiModeFilterParams() : Cutoff(0.99),
									  Resonance(0.0),
									  Mode(FILTER_MODE_LOWPASS)
			{
				CalculateFeedbackAmount();
			}

			inline void SetPoles(FilterPoles poles)
			{
				Poles = poles;
			}

			inline void SetCutoff(float cutoff)
			{
				Cutoff = cutoff;
				CalculateFeedbackAmount();
			};

			inline void SetResonance(float resonance)
			{
				Resonance = fmaxf(fminf(resonance, 0.9999), 0.0001);
				CalculateFeedbackAmount();
			};
			inline void SetFilterMode(FilterMode mode) { Mode = mode; }

		private:
			inline void CalculateFeedbackAmount() { FeedbackAmount = Resonance + Resonance / (1.0f - Cutoff); }
		};
		
		template<size_t SAMPLERATE>
		class MultiModeFilter : public FxElement<MultiModeFilterParams, SAMPLERATE>
		{
		public:
			MultiModeFilter(MultiModeFilterParams *params) : FxElement<MultiModeFilterParams, SAMPLERATE>(params)

			{
				buf0 = {0, 0};
				buf1 = {0, 0};
				buf2 = {0, 0};
				buf3 = {0, 0};
			};

			inline LRSample32F Process(LRSample32F sample) override
			{
				buf0 = buf0 + ((sample - buf0) * this->Params->Cutoff);
				buf1 = buf1 + ((buf0 - buf1) * this->Params->Cutoff);
				buf2 = buf2 + ((buf1 - buf2) * this->Params->Cutoff);
				buf3 = buf3 + ((buf2 - buf3) * this->Params->Cutoff);

				switch (this->Params->Mode)
				{
				case FILTER_MODE_LOWPASS:
					return this->Params->Poles == FILTER_POLES_TWO ? buf1 : buf3;
				case FILTER_MODE_HIGHPASS:
					return sample - (this->Params->Poles == FILTER_POLES_TWO ? buf0 : buf2);
				case FILTER_MODE_BANDPASS:
					return this->Params->Poles == FILTER_POLES_TWO ? (buf0 - buf1) : (buf0 - buf3);
				default:
					return LRSample32F{0, 0};
				}
			}

		private:
			LRSample32F buf0;
			LRSample32F buf1;
			LRSample32F buf2;
			LRSample32F buf3;
		};
	} // namespace Dsp
} // namespace ByteFarm