#pragma once
#include <sstream>
#include <Runtime/Object.h>
#include <Runtime/MagicMethodNames.h>
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
            Object* float_object_add_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* float_object_sub_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* float_object_mul_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* float_object_div_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* float_object_equal_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* float_object_str_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* float_object_bool_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* float_object_greater_than_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* float_object_greater_equal_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* float_object_less_than_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* float_object_less_equal_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* float_object_as_method(Object** args, size_t arg_num, bool& exception_thrown);

            class FloatType : public Type
            {
            public:
                FloatType();

                static FloatType* get_instance();
            };

            using FloatValue = double;

            class FloatObject : public Object
            {
            public:
                const double value;

                FloatObject();
                FloatObject(double value);

                static FloatType* static_type();
            };

        }
    }
}