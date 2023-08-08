#pragma once
#include <Runtime/BasicObjectDefs.h>
#include <Runtime/Builtin/BuiltinFunction.h>
#include <Runtime/Builtin/BuiltinException.h>
#include <Runtime/MagicMethodNames.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            inline Object* bool_object_equal(Object** args, size_t arg_num, bool& exception_thrown);  // 实现于Runtime/Builtin/Int.hpp
            inline Object* bool_object_str(Object** args, size_t arg_num, bool& exception_thrown);  // 实现于Runtime/Builtin/String.hpp

            class BoolType : public Type
            {
            public:
                BoolType();

                static BoolType* get_instance();
            };

            using BoolValue = bool;

            class BoolObject : public Object
            {
            public:
                const BoolValue value;

                static BoolObject* get_instance(bool value);
                static BoolType* static_type();
            
            private:
                BoolObject(bool value);
            };
        }
    }
}