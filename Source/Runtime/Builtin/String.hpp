#pragma once
#include <Runtime/Object.hpp>
#include <Runtime/MagicMethodNames.hpp>
#include <Runtime/Builtin/Bool.hpp>
#include <Runtime/Builtin/BuiltinFunction.hpp>
#include <Runtime/Builtin/BuiltinException.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            inline Object* string_object_add(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* string_object_equal(Object** args, size_t arg_num, bool& exception_thrown);
            inline Object* string_object_bool(Object** args, size_t arg_num, bool& exception_thrown);
            
            class StringType : public Type
            {
            public:
                StringType();
            };

            StringType::StringType()
            {
                this->name = "string";
                this->attrs[MagicMethodNames::add_method_name] = new BuiltinFunction(string_object_add);
                this->attrs[MagicMethodNames::equal_method_name] = new BuiltinFunction(string_object_equal);
                this->attrs[MagicMethodNames::bool_method_name] = new BuiltinFunction(string_object_bool);
            }

            StringType* string_type_instance = new StringType;
            
            class StringObject : public Object
            {
            public:
                std::string value;

                StringObject();
            };

            StringObject::StringObject()
            {
                this->type = string_type_instance;
            }

            Object* string_object_add(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                StringObject* self = dynamic_cast<StringObject*>(args[0]);
                StringObject* other = dynamic_cast<StringObject*>(args[1]);
                if (self == nullptr || other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }
                StringObject* result_obj = new StringObject;
                result_obj->value = self->value + other->value;
                return result_obj;
            }

            Object* string_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                StringObject* self = dynamic_cast<StringObject*>(args[0]);
                StringObject* other = dynamic_cast<StringObject*>(args[1]);
                if (self == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }
                if (other == nullptr)
                {
                    return new BoolObject(false);
                }
                return new BoolObject(self->value == other->value);
            }

            Object* string_object_bool(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 1)
                {
                    exception_thrown = true;
                    return new BuiltinException("Excepted 2 arguments, got " + std::to_string(arg_num));
                }
                StringObject* self = dynamic_cast<StringObject*>(args[0]);
                if (self == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Types of argument 0 must be 'string'" + std::to_string(arg_num));
                }

                return new BoolObject(!self->value.empty());
            }


            /* 为防止循环include，部分object的__str__方法放在这里实现 */            
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
            
            Object* builtin_function_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                StringObject* str = new StringObject;
                str->value = "<builtin-function>";
                return str;
            }
        }  // namespace Builtin
    }  // namespace Runtime
}  // namespace XyA