#pragma once
#include <Runtime/Object.hpp>
#include <Runtime/Builtin/BuiltinFunction.hpp>
#include <Runtime/Builtin/BuiltinException.hpp>
#include <Runtime/MagicMethodNames.hpp>


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
            };

            BoolType::BoolType()
            {
                this->name = "bool";
                this->attrs[MagicMethodNames::equal_method_name] = new BuiltinFunction(bool_object_equal);
                this->attrs[MagicMethodNames::str_method_name] = new BuiltinFunction(bool_object_str);
            }

            BoolType* bool_type_instace = new BoolType;

            class BoolObject : public Object
            {
            public:
                bool value;

                BoolObject(bool value=false);
            };

            BoolObject::BoolObject(bool value)
            {
                this->type = bool_type_instace;
                this->value = value;
            }
        }
    }
}