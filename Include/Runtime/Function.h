#pragma once
#include <Runtime/Object.h>
#include <Runtime/CodeObject.h>


namespace XyA
{
    namespace Runtime
    {
        class Function : public Object
        {
        public:
            CodeObject* code_object;
        };
    }
}