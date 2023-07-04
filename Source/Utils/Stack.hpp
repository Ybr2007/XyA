#pragma once
#include <stddef.h>
#include <stdlib.h>


namespace XyA
{
    namespace Utils
    {
        template <typename T>
        class Stack
        {
        public:
            Stack()
            {
                this->__items = reinterpret_cast<T*>(malloc(sizeof(T)));
            }

            ~Stack()
            {
                free(this->__items);
            }

            void push(const T& object)
            {
                if (++ this->__size > this->__capacity)
                {
                    this->__expand();
                }

                this->__items[this->__size - 1] = object;
            }

            const T& top() const
            {
                return this->__items[this->__size - 1];
            }

            void set_top(const T& object)
            {
                this->__items[this->__size - 1] = object;
            }

            T pop()
            {
                return this->__items[-- this->__size];
            }

            bool empty() const
            {
                return this->__size == 0;
            }

            size_t size() const
            {
                return this->__size;
            }

        private:
            size_t __size = 0;
            size_t __capacity = 1;
            T* __items;

            void __expand()
            {
                this->__capacity *= 2;
                this->__items = reinterpret_cast<T*>(realloc(this->__items, this->__capacity * sizeof(T)));
                if (this->__items == nullptr)
                {
                    exit(-2);
                }
            }
        };
    }
}