// CircularBuffer.h
#pragma once
#include "TypedArray.hpp"

namespace ByteFarm
{
	namespace Dsp
	{

		template <class T, size_t S, class intType>
		class CircularBuffer : public TypedArray<T, S, intType>
		{

			volatile long _writeIndex = 0;



		public:
			CircularBuffer() : TypedArray<T, S, intType>()
			{
			}

			T Read(int32_t offset)
			{
				long idx = (_writeIndex + offset) % S;
				return  this->Get(idx);
			}


			void Write(T value, int32_t subOffset = 0)
			{
				this->Set((_writeIndex + subOffset) % S, value);
			}

			void Increment()
			{
				//_writeIndex = (_writeIndex++ % S);
				_writeIndex++;
				_writeIndex%=S;
			}
		};
	} // namespace Dsp
} // namespace ByteFarm