// CircularBuffer.h
#pragma once
#include <array>

namespace ByteFarm
{
	namespace Dsp
	{

		template <class T, std::size_t S>
		class CircularBuffer
		{
			std::array<T, S> _buffer();

			volatile long _writeIndex = 0;

			CircularBuffer()
			{
			}

		public:
			T Read(long offset)
			{
				long idx = (_writeIndex + offset) % _buffer.size();

				idx = idx > 0 ? idx : _buffer.size() + idx;

				return _buffer[idx];
			}

			void Write(T value)
			{
				_buffer[_writeIndex] = value;
			}

			void Increment()
			{
				_writeIndex = ++_writeIndex % _buffer.size();
			}
		};
	} // namespace Dsp
} // namespace ByteFarm