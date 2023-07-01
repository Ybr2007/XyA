#pragma once
#include <Runtime/Builtin/Null.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            NullType::NullType()
            {
                this->name = "null";
                this->type = nullptr;
                this->attrs[MagicMethodNames::equal_method_name] = XyA_Allocate(BuiltinFunction, null_object_equal);
                this->attrs[MagicMethodNames::str_method_name] = XyA_Allocate(BuiltinFunction, null_object_str);

                for (const auto& iter : this->attrs)
                {
                    iter.second->reference();
                }
            }
            
            NullType* NullType::get_instance()
            {
                static NullType instance;
                return &instance;
            }

            NullObject::NullObject(bool value)
            {
                this->type = NullType::get_instance();
            }

            NullObject* NullObject::get_instance()
            {
                static NullObject* instance = XyA_Allocate_(NullObject);
                return instance;
            }

            Object* null_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, "Expected 2 arguments, got " + std::to_string(arg_num));
                }
                NullObject* self = dynamic_cast<NullObject*>(args[0]);

                NullObject* other = dynamic_cast<NullObject*>(args[1]);

                if (self == nullptr)
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, "Type Error");
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
                    return XyA_Allocate(BuiltinException, "Excepted 1 argument, got " + std::to_string(arg_num));
                }

                NullObject* self = dynamic_cast<NullObject*>(args[0]);
                if (self == nullptr)
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, "The type of argument 'self' must be int." + std::to_string(arg_num));
                }

                StringObject* str = new StringObject;
                str->value = "null";

                return str;
            }
        }
    }
}