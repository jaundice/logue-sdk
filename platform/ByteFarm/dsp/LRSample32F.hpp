#pragma once
namespace ByteFarm
{
	namespace Dsp
	{
		struct LRSample32F
		{
			float Left;
			float Right;

			LRSample32F Clone() const
			{
				return LRSample32F{Left, Right};
			}

			LRSample32F MonoSum() const
			{
				float s = (Left + Right) * 0.5f;

				return LRSample32F{s, s};
			}

			LRSample32F operator*(float factor) const
			{
				return LRSample32F{Left * factor, Right * factor};
			}

			LRSample32F operator+(LRSample32F other) const
			{
				return LRSample32F{Left + other.Left, Right + other.Right};
			}

			LRSample32F operator-(LRSample32F other) const
			{
				return LRSample32F{Left - other.Left, Right - other.Right};
			}

			LRSample32F operator!() const
			{
				return LRSample32F{Left * -1.f, Right * -1.f};
			}
		};

		struct LRCSample32F
		{
			float Left;
			float Right;
			float Center;

			LRCSample32F operator*(float factor) const
			{
				return LRCSample32F{Left * factor, Right * factor, Center * factor};
			}

			LRCSample32F operator+(LRCSample32F other) const
			{
				return LRCSample32F{Left + other.Left, Right + other.Right, Center + other.Center};
			}

			LRCSample32F operator-(LRCSample32F other) const
			{
				return LRCSample32F{Left - other.Left, Right - other.Right, Center - other.Center};
			}
			LRCSample32F operator!() const
			{
				return LRCSample32F{Left * -1.f, Right * -1.f, Center * -1.f};
			}
		};

		class SampleConvert
		{
		public:
			static LRCSample32F ToLRC(LRSample32F sample)
			{

				float mid = 0.5 * (sample.Left + sample.Right);

				return LRCSample32F{
					sample.Left - mid,
					sample.Right - mid,
					mid};
			}

			static LRSample32F ToLR(LRCSample32F sample)
			{
				return LRSample32F{
					sample.Left + sample.Center,
					sample.Right + sample.Center};
			}
		};

	} // namespace Dsp
} // namespace ByteFarm
