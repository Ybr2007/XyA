#pragma once
#include <Runtime/Builtin/Bool.h>
#include <Runtime/Builtin/Int.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            Object* bool_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(BoolObject)

                BoolObject* bool_other = dynamic_cast<BoolObject*>(args[1]);
                IntObject* int_other = nullptr;
                if (bool_other == nullptr)
                {
                    int_other = dynamic_cast<IntObject*>(args[1]);
                }

                if (bool_other == nullptr && int_other == nullptr)
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, "Type Error");
                }

                if (bool_other != nullptr)
                {
                    return XyA_Allocate(BoolObject, self->value == bool_other->value);
                }
                else  // int_other != nullptr
                {
                    return XyA_Allocate(BoolObject, self->value == int_other->value);
                }
            }
            
            Object* bool_object_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                XyA_Builtin_Method_Get_Self(BoolObject)

                StringObject* str = XyA_Allocate_(StringObject);
                str->value = self->value ? "true" : "false";

                return str;
            }

            BoolType::BoolType()
            {
                this->name = "bool";
                this->set_attr(MagicMethodNames::equal_method_name, XyA_Allocate(BuiltinFunction, bool_object_equal));
                this->set_attr(MagicMethodNames::str_method_name, XyA_Allocate(BuiltinFunction, bool_object_str));
            }

            BoolType* BoolType::get_instance()
            {
                static BoolType instance;
                return &instance;
            }

            BoolObject::BoolObject(bool value)
            {
                this->__type = BoolType::get_instance();
                this->value = value;
            }
        }
    }
}