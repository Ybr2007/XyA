#pragma once
#include <Runtime/BasicObjectDefs.h>
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
                static NullObject* get_instance();
                static NullType* static_type();

            private:
                NullObject();
            };
        }
    }
}