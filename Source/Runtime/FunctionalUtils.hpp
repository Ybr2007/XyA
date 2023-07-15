#pragma once
#include <Runtime/Object.h>
#include <Runtime/CustomFunction.h>
#include <Runtime/Builtin/BuiltinFunction.h>


namespace XyA
{
    namespace Runtime
    {
        inline bool is_builtin_function(Object* object)
        {
            return object->type() == Builtin::BuiltinFunctionType::get_instance();
        }

        inline bool is_custom_function(Object* object)
        {
            return object->type() == FunctionType::get_instance();
        }
        
        inline bool is_function(Object* object)
        {
            return is_builtin_function(object) || is_custom_function(object);
        }

        inline bool is_callablel(Object* object)
        {
            return is_function(object);
        }
    }
}