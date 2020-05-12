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
				float s = (Left + Right) / 2.0f;

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
		};
	} // namespace Dsp
} // namespace ByteFarm
