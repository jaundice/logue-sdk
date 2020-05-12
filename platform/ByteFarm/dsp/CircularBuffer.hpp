// CircularBuffer.h
#pragma once
#include "TypedArray.hpp"

namespace ByteFarm
{
	namespace Dsp
	{

		template <class T, std::size_t S>
		class CircularBuffer : public TypedArray<T, S>
		{

			volatile long _writeIndex = 0;

			CircularBuffer() : TypedArray<T, S>()
			{
			}

		public:
			T Read(long offset)
			{
				long idx = (_writeIndex + offset);
				return Get(idx);
			}

			void Write(T value)
			{
				Set(_writeIndex % S, value);
			}

			void Increment()
			{
				_writeIndex = ++_writeIndex % S;
			}
		};
	} // namespace Dsp
} // namespace ByteFarm