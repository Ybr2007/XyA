#pragma once
#include <Runtime/Builtin/String.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            StringType::StringType()
            {
                this->name = "string";
                this->type = nullptr;
                this->attrs[MagicMethodNames::add_method_name] = XyA_Allocate(BuiltinFunction, string_object_add);
                this->attrs[MagicMethodNames::equal_method_name] = XyA_Allocate(BuiltinFunction, string_object_equal);
                this->attrs[MagicMethodNames::bool_method_name] = XyA_Allocate(BuiltinFunction, string_object_bool);

                for (const auto& iter : this->attrs)
                {
                    iter.second->reference();
                }
            }

            StringType* StringType::get_instance()
            {
                static StringType instance;
                return &instance;
            }


            StringObject::StringObject()
            {
                this->type = StringType::get_instance();
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
            
        }  // namespace Builtin
    }  // namespace Runtime
}  // namespace XyA