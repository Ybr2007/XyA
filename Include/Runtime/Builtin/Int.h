#pragma once
#include <Runtime/BasicObjectDefs.h>
#include <Runtime/MagicMethodNames.h>
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
            Object* int_object_add_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* int_object_sub_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* int_object_mul_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* int_object_divide(Object** args, size_t arg_num, bool& exception_thrown);
            Object* int_object_equal(Object** args, size_t arg_num, bool& exception_thrown);
            Object* int_object_str_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* int_object_bool_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* int_object_greater_than_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* int_object_greater_equal_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* int_object_less_than_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* int_object_less_equal_method(Object** args, size_t arg_num, bool& exception_thrown);
            Object* int_object_as_method(Object** args, size_t arg_num, bool& exception_thrown);

            class IntType : public Type
            {
            public:
                IntType();

                static IntType* get_instance();
            };

            using IntValue = long long;

            class IntObject : public Object
            {
            public:
                const long long value;

                IntObject();
                IntObject(long long value);

                static IntType* static_type();

                #ifdef Debug_Display_Object
                std::string to_string() const;
                #endif
            };

        }  // namespace Builtin
    }  // namespace Runtime
}  // namespace XyA