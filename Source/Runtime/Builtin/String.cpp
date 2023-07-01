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
                XyA_Function_Check_Arg_Num(2)
                XyA_Method_Get_Self(StringObject)

                StringObject* other = dynamic_cast<StringObject*>(args[1]);
                if (other == nullptr)
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, "Type Error");
                }
                StringObject* result_obj = XyA_Allocate_(StringObject);
                result_obj->value = self->value + other->value;
                return result_obj;
            }

            Object* string_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Method_Get_Self(StringObject)

                StringObject* other = dynamic_cast<StringObject*>(args[1]);
                if (other == nullptr)
                {
                    return XyA_Allocate(BoolObject, false);
                }
                return XyA_Allocate(BoolObject, self->value == other->value);
            }

            Object* string_object_bool(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                XyA_Method_Get_Self(StringObject)

                return XyA_Allocate(BoolObject, !self->value.empty());
            }
            
        }  // namespace Builtin
    }  // namespace Runtime
}  // namespace XyA