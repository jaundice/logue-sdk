#pragma once
#include <initializer_list>
//#include <assert.h>
#include <type_traits>
#include <stdlib.h>
namespace ByteFarm
{
    namespace Dsp
    {

        template <class T>
        void Delete(T item)
        {
            //do nothing
        }

        template <class T>
        void Delete(T *item)
        {
            delete item;
        }

        template <class T, size_t sz, class intType>
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
                intType i = 0;

                for (const T *t = input.begin(); t < input.end(), i < sz; t++)
                {
                    Set(i, *t);
                    i++;
                }
            }
            inline T Get(intType idx)
            {
                return _array[idx % sz];
            }

            inline void Set(intType idx, T value)
            {
                _array[idx % sz] = value;
            }

            inline intType Size()
            {
                return sz;
            }

            inline const T *Ptr()
            {
                return _array;
            }

            ~TypedArray()
            {
                for (intType i = 0; i < sz; i++)
                {
                    Delete<T>(_array[i]);
                }

                delete _array;
            }
        };

    } // namespace Dsp
} // namespace ByteFarm
