#pragma once
#include <Runtime/Builtin/Bool.h>
#include <Runtime/Builtin/Int.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            Object* bool_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                BoolObject* self = dynamic_cast<BoolObject*>(args[0]);

                BoolObject* bool_other = dynamic_cast<BoolObject*>(args[1]);
                IntObject* int_other = nullptr;
                if (bool_other == nullptr)
                {
                    int_other = dynamic_cast<IntObject*>(args[1]);
                }

                if (self == nullptr || (bool_other == nullptr && int_other == nullptr))
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }

                if (bool_other != nullptr)
                {
                    return new BoolObject(self->value == bool_other->value);
                }
                else  // int_other != nullptr
                {
                    return new BoolObject(self->value == int_other->value);
                }
            }
            
            Object* bool_object_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 1)
                {
                    exception_thrown = true;
                    return new BuiltinException("Excepted 1 argument, got " + std::to_string(arg_num));
                }

                BoolObject* self = dynamic_cast<BoolObject*>(args[0]);
                if (self == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("The type of argument 'self' must be int." + std::to_string(arg_num));
                }

                StringObject* str = new StringObject;
                str->value = self->value ? "true" : "false";

                return str;
            }

            BoolType::BoolType()
            {
                this->name = "bool";
                this->type = nullptr;
                this->attrs[MagicMethodNames::equal_method_name] = new BuiltinFunction(bool_object_equal);
                this->attrs[MagicMethodNames::str_method_name] = new BuiltinFunction(bool_object_str);
            }

            BoolType* BoolType::get_instance()
            {
                static BoolType* instance = new BoolType;
                return instance;
            }

            BoolObject::BoolObject(bool value)
            {
                this->type = BoolType::get_instance();
                this->value = value;
            }
        }
    }
}