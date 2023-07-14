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
            BoolType::BoolType()
            {
                this->name = "bool";
                this->ref_count_enabled = false;
                this->set_attr(MagicMethodNames::equal_method_name, XyA_Allocate(BuiltinFunction, bool_object_equal));
                this->set_attr(MagicMethodNames::str_method_name, XyA_Allocate(BuiltinFunction, bool_object_str));
            }

            BoolType* BoolType::get_instance()
            {
                static BoolType instance;
                return &instance;
            }

            BoolObject::BoolObject(bool value_) : value(value_)
            {
                this->ref_count_enabled = false;
                this->__type = BoolType::get_instance();
            }

            BoolObject* BoolObject::get_instance(bool value)
            {
                static BoolObject true_instance = BoolObject(true);
                static BoolObject flase_instance = BoolObject(false);
                return value ? &true_instance : &flase_instance;
            }

            BoolType* BoolObject::static_type()
            {
                return BoolType::get_instance();
            }

            Object* bool_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(BoolObject)

                IntObject* int_other = nullptr;
                BoolObject* bool_other = nullptr;
                if (args[1]->is_instance(IntType::get_instance()))
                {
                    int_other = dynamic_cast<IntObject*>(args[1]);
                }
                else if (args[1]->is_instance(BoolType::get_instance()))
                {
                    bool_other = dynamic_cast<BoolObject*>(args[1]);
                }
                else
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, "Type Error");
                }

                return BoolObject::get_instance(self->value == (bool_other != nullptr ? bool_other->value : int_other->value));
            }
            
            Object* bool_object_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                XyA_Builtin_Method_Get_Self(BoolObject)

                StringObject* str = XyA_Allocate_(StringObject);
                str->value = self->value ? "true" : "false";

                return str;
            }
        }
    }
}