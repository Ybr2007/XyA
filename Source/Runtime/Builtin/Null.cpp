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
                Type::Type("null_t");
                this->set_attr(MagicMethodNames::equal_method_name_id, XyA_Allocate(BuiltinFunction, null_object_equal));
                this->set_attr(MagicMethodNames::str_method_name_id, XyA_Allocate(BuiltinFunction, null_object_str));
            }
            
            NullType* NullType::get_instance()
            {
                static NullType instance;
                return &instance;
            }

            NullObject::NullObject()
            {
                this->ref_count_enabled = false;
                this->__type = NullType::get_instance();
            }

            NullObject* NullObject::get_instance()
            {
                static NullObject instance;
                return &instance;
            }

            NullType* NullObject::static_type()
            {
                return NullType::get_instance();
            }

            Object* null_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(NullObject);

                return BoolObject::get_instance(args[1]->is_instance(NullType::get_instance()));
            }

            Object* null_object_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                XyA_Builtin_Method_Get_Self(NullObject);

                StringObject* str = XyA_Allocate_(StringObject);
                str->value = "null";

                return str;
            }
        }
    }
}