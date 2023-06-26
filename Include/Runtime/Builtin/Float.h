#pragma once
#include <sstream>
#include <Runtime/Object.h>
#include <Runtime/MagicMethodNames.hpp>
#include <Runtime/Builtin/BuiltinFunction.h>
#include <Runtime/Builtin/Bool.h>
#include <Runtime/Builtin/BuiltinException.h>
#include <Runtime/Builtin/String.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            inline Object* float_object_add(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* float_object_subtract(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* float_object_multiply(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* float_object_divide(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* float_object_equal(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* float_object_str(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* float_object_bool(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* float_object_compare_if_greater(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* float_object_compare_if_greater_equal(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* float_object_compare_if_less(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* float_object_compare_if_less_equal(Object** args, size_t arg_num, bool& exception_thrown);

            class FloatType : public Type
            {
            public:
                FloatType();

                static FloatType* get_instance();
            };

            class FloatObject : public Object
            {
            public:
                double value;

                FloatObject();
            };

        }
    }
}