#pragma once
#include <Runtime/Object.h>
#include <Runtime/Builtin/Bool.h>
#include <Runtime/Builtin/String.h>
#include <Runtime/Builtin/BuiltinFunction.h>
#include <Runtime/Builtin/BuiltinException.h>
#include <Runtime/MagicMethodNames.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            inline Object* null_object_equal(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* null_object_str(Object** args, size_t arg_num, bool& exception_thrown);

            class NullType : public Type
            {
            public:
                NullType();

                static NullType* get_instance();
            };

            class NullObject : public Object
            {
            public:
                NullObject(bool value=false);

                static NullObject* get_instance();
            };
        }
    }
}