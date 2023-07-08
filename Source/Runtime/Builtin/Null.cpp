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
                this->magic_methods[MagicMethodNames::equal_method_index] = XyA_Allocate(BuiltinFunction, null_object_equal);
                this->magic_methods[MagicMethodNames::str_method_index] = XyA_Allocate(BuiltinFunction, null_object_str);

                this->reference_attrs();
            }
            
            NullType* NullType::get_instance()
            {
                static NullType instance;
                return &instance;
            }

            NullObject::NullObject(bool value)
            {
                this->__type = NullType::get_instance();
            }

            NullObject* NullObject::get_instance()
            {
                static NullObject* instance = XyA_Allocate_(NullObject);
                return instance;
            }

            Object* null_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Method_Get_Self(NullObject);

                NullObject* other = dynamic_cast<NullObject*>(args[1]);

                if (other != nullptr)
                {
                    return XyA_Allocate(BoolObject, true);
                }
                else  // other == nullptr
                {
                    return XyA_Allocate(BoolObject, false);
                }
            }

            Object* null_object_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                XyA_Method_Get_Self(NullObject);

                StringObject* str = XyA_Allocate_(StringObject);
                str->value = "null";

                return str;
            }
        }
    }
}