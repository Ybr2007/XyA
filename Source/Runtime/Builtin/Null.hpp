#pragma once
#include <Runtime/Object.hpp>
#include <Runtime/Builtin/Bool.hpp>
#include <Runtime/Builtin/String.hpp>
#include <Runtime/Builtin/BuiltinFunction.hpp>
#include <Runtime/Builtin/BuiltinException.hpp>
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
            };

            NullType::NullType()
            {
                this->name = "null";
                this->attrs[MagicMethodNames::equal_method_name] = new BuiltinFunction(null_object_equal);
                this->attrs[MagicMethodNames::str_method_name] = new BuiltinFunction(null_object_str);
            }

            NullType* null_type_instace = new NullType;

            class NullObject : public Object
            {
            public:
                NullObject(bool value=false);
            };

            NullObject* null_object_instance = new NullObject;

            NullObject::NullObject(bool value)
            {
                this->type = null_type_instace;
            }

            Object* null_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                NullObject* self = dynamic_cast<NullObject*>(args[0]);

                NullObject* other = dynamic_cast<NullObject*>(args[1]);

                if (self == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }

                if (other != nullptr)
                {
                    return new BoolObject(true);
                }
                else  // other == nullptr
                {
                    return new BoolObject(false);
                }
            }

            Object* null_object_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 1)
                {
                    exception_thrown = true;
                    return new BuiltinException("Excepted 1 argument, got " + std::to_string(arg_num));
                }

                NullObject* self = dynamic_cast<NullObject*>(args[0]);
                if (self == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("The type of argument 'self' must be int." + std::to_string(arg_num));
                }

                StringObject* str = new StringObject;
                str->value = "null";

                return str;
            }
        }
    }
}