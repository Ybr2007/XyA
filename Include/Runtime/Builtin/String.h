#pragma once
#include <Runtime/BasicObjectDefs.h>
#include <Runtime/MagicMethodNames.h>
#include <Runtime/Builtin/Bool.h>
#include <Runtime/Builtin/BuiltinFunction.h>
#include <Runtime/Builtin/BuiltinException.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            inline Object* string_object_add(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* string_object_multiply(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* string_object_equal(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* string_object_bool(Object** args, size_t arg_num, bool& exception_thrown);
            
            class StringType : public Type
            {
            public:
                StringType();

                static StringType* get_instance();
            };

            using StringValue = StringSource;
            
            class StringObject : public Object
            {
            public:
                const StringValue value;

                StringObject();
                StringObject(StringView value);

                static StringType* static_type();
            };
            
        }  // namespace Builtin
    }  // namespace Runtime
}  // namespace XyA