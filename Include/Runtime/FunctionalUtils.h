#pragma once
#include <Runtime/Object.h>
#include <Runtime/Function.h>
#include <Runtime/Builtin/BuiltinFunction.h>


namespace XyA
{
    namespace Runtime
    {
        inline bool is_function(Object* object);
        inline bool is_builtin_function(Object* object);
        inline bool is_custom_function(Object* object);
    }
}