#pragma once
#include <Runtime/Object.h>
#include <Runtime/MagicMethodNames.hpp>
#include <Runtime/Builtin/Bool.h>
#include <Runtime/Builtin/Float.h>
#include <Runtime/Builtin/String.h>
#include <Runtime/Builtin/BuiltinFunction.h>
#include <Runtime/Builtin/BuiltinException.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            inline Object* int_object_add(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* int_object_subtract(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* int_object_multiply(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* int_object_divide(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* int_object_equal(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* int_object_str(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* int_object_bool(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* int_object_compare_if_greater(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* int_object_compare_if_greater_equal(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* int_object_compare_if_less(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* int_object_compare_if_less_equal(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* int_object_as(Object** args, size_t arg_num, bool& exception_thrown);

            class IntType : public Type
            {
            public:
                IntType();

                static IntType* get_instance();
            };

            class IntObject : public Object
            {
            public:
                long long value;

                IntObject();
                IntObject(long long value);

                #ifdef Debug_Display_Object
                std::string to_string() const;
                #endif
            };

        }  // namespace Builtin
    }  // namespace Runtime
}  // namespace XyA