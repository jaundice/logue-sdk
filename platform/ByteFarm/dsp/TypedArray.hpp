#pragma once
#include <initializer_list>
//#include <assert.h>
#include <type_traits>
namespace ByteFarm
{
    namespace Dsp
    {

        template <class T, size_t sz>
        class TypedArray
        {
        private:
            T *_array;

        public:
            TypedArray()
            {
                _array = (T *)(void(*))malloc(sizeof(T) * sz);
            }
            TypedArray(std::initializer_list<T> input) : TypedArray()
            {
                int32_t i = 0;

                for (const T *t = input.begin(); t < input.end(), i < sz; t++)
                {
                    Set(i, *t);
                    i++;
                }
            }
            T Get(uint32_t idx)
            {
                return _array[idx % sz];
            }

            void Set(uint32_t idx, T value)
            {
                _array[idx % sz] = value;
            }

            int32_t size()
            {
                return sz;
            }

            T *data()
            {
                return _array;
            }

            ~TypedArray()
            {
                if (std::is_class<T>::value && std::is_pointer<T>::value)
                {
                    for (int32_t i = 0; i < sz; i++)
                    {
#warning need to implement element deletion but currently the is_class and is_pointer tests aren't working'
                        //delete _array[i];
                    }
                }
                delete _array;
            }
        };
    } // namespace Dsp
} // namespace ByteFarm
