#include <Runtime/Builtin/String.h>
#include <Runtime/Builtin/Int.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            StringType::StringType()
            {
                Type::Type("string");
                this->ref_count_enabled = false;
                this->set_attr(MagicMethodNames::add_method_name, XyA_Allocate(BuiltinFunction, string_object_add));
                this->set_attr(MagicMethodNames::multiply_method_name, XyA_Allocate(BuiltinFunction, string_object_multiply));
                this->set_attr(MagicMethodNames::equal_method_name, XyA_Allocate(BuiltinFunction, string_object_equal));
                this->set_attr(MagicMethodNames::bool_method_name, XyA_Allocate(BuiltinFunction, string_object_bool));
            }

            StringType* StringType::get_instance()
            {
                static StringType instance;
                return &instance;
            }

            StringObject::StringObject()
            {
                this->__type = StringType::get_instance();
            }

            StringObject::StringObject(const std::string& value)
            {
                this->__type = StringType::get_instance();
                this->value = value;
            }

            StringType* StringObject::static_type()
            {
                return StringType::get_instance();
            }

            Object* string_object_add(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(StringObject)

                StringObject* other = nullptr;
                if (!args[1]->is_instance(StringType::get_instance()))
                {
                    other = static_cast<StringObject*>(args[1]);
                }
                else
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, "Type Error");
                }
                StringObject* result_obj = XyA_Allocate_(StringObject);
                result_obj->value = self->value + other->value;
                return result_obj;
            }

            Object* string_object_multiply(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(StringObject)

                IntObject* other = nullptr;
                if (!args[1]->is_instance(IntType::get_instance()))
                {
                    other = static_cast<IntObject*>(args[1]);
                }
                else
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, "Type Error");
                }
                StringObject* result_obj = XyA_Allocate_(StringObject);
                result_obj->value = "";
                for (size_t i = 0; i < other->value; i ++)
                {
                    result_obj->value += self->value;
                }
                return result_obj;
            }

            Object* string_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(StringObject)

                StringObject* other = nullptr;
                if (!args[1]->is_instance(StringType::get_instance()))
                {
                    other = static_cast<StringObject*>(args[1]);
                }
                else
                {
                    return BoolObject::get_instance(false);
                }
                return BoolObject::get_instance(self->value == other->value);
            }

            Object* string_object_bool(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                XyA_Builtin_Method_Get_Self(StringObject)

                return BoolObject::get_instance(!self->value.empty());
            }
            
        }  // namespace Builtin
    }  // namespace Runtime
}  // namespace XyA