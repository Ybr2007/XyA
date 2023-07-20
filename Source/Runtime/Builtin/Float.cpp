#include <format>
#include <Runtime/Builtin/Float.h>
#include <Runtime/Builtin/Int.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            FloatType::FloatType()
            {
                Type::Type("float");
                this->ref_count_enabled = false;
                this->set_attr(MagicMethodNames::add_method_name, XyA_Allocate(BuiltinFunction, float_object_add_method));
                this->set_attr(MagicMethodNames::subtract_method_name, XyA_Allocate(BuiltinFunction, float_object_sub_method));
                this->set_attr(MagicMethodNames::multiply_method_name, XyA_Allocate(BuiltinFunction, float_object_mul_method));
                this->set_attr(MagicMethodNames::divide_method_name, XyA_Allocate(BuiltinFunction, float_object_div_method));
                this->set_attr(MagicMethodNames::equal_method_name, XyA_Allocate(BuiltinFunction, float_object_equal_method));
                this->set_attr(MagicMethodNames::str_method_name, XyA_Allocate(BuiltinFunction, float_object_str_method));
                this->set_attr(MagicMethodNames::bool_method_name, XyA_Allocate(BuiltinFunction, float_object_bool_method));
                this->set_attr(MagicMethodNames::greater_method_name, XyA_Allocate(BuiltinFunction, float_object_greater_than_method));
                this->set_attr(MagicMethodNames::greater_equal_method_name, XyA_Allocate(BuiltinFunction, float_object_greater_equal_method));
                this->set_attr(MagicMethodNames::less_method_name, XyA_Allocate(BuiltinFunction, float_object_less_than_method));
                this->set_attr(MagicMethodNames::less_equal_method_name, XyA_Allocate(BuiltinFunction, float_object_less_equal_method));
                this->set_attr(MagicMethodNames::as_method_name, XyA_Allocate(BuiltinFunction, float_object_as_method));
            }

            FloatType* FloatType::get_instance()
            {
                static FloatType instance;
                return &instance;
            }

            FloatObject::FloatObject()
            {
                this->__type = FloatType::get_instance();
            }

            FloatType* FloatObject::static_type()
            {
                return FloatType::get_instance();
            }

            /* Float Methods */
            Object* float_object_add_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(FloatObject)

                IntObject* int_other = nullptr;
                FloatObject* float_other = nullptr;
                if (args[1]->is_instance(IntType::get_instance()))
                {
                    int_other = static_cast<IntObject*>(args[1]);
                }
                else if (args[1]->is_instance(FloatType::get_instance()))
                {
                    float_other = static_cast<FloatObject*>(args[1]);
                }
                else
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, 
                        std::format("Unsupport operand type(s) for +: 'float' and '{}'", args[1]->type()->name));
                }

                FloatObject* result_obj = XyA_Allocate_(FloatObject);
                result_obj->value = self->value + ((int_other != nullptr) ? int_other->value : float_other->value);
                return result_obj; 
            }

            Object* float_object_sub_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(FloatObject)

                IntObject* int_other = nullptr;
                FloatObject* float_other = nullptr;
                if (args[1]->is_instance(IntType::get_instance()))
                {
                    int_other = static_cast<IntObject*>(args[1]);
                }
                else if (args[1]->is_instance(FloatType::get_instance()))
                {
                    float_other = static_cast<FloatObject*>(args[1]);
                }
                else
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, 
                        std::format("Unsupport operand type(s) for -: 'float' and '{}'", args[1]->type()->name));
                }

                FloatObject* result_obj = XyA_Allocate_(FloatObject);
                result_obj->value = self->value - ((int_other != nullptr) ? int_other->value : float_other->value);
                return result_obj;             
            }

            Object* float_object_mul_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(FloatObject)

                IntObject* int_other = nullptr;
                FloatObject* float_other = nullptr;
                if (args[1]->is_instance(IntType::get_instance()))
                {
                    int_other = static_cast<IntObject*>(args[1]);
                }
                else if (args[1]->is_instance(FloatType::get_instance()))
                {
                    float_other = static_cast<FloatObject*>(args[1]);
                }
                else
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, 
                        std::format("Unsupport operand type(s) for *: 'float' and '{}'", args[1]->type()->name));
                }

                FloatObject* result_obj = XyA_Allocate_(FloatObject);
                result_obj->value = self->value * ((int_other != nullptr) ? int_other->value : float_other->value);
                return result_obj;
            }

            Object* float_object_div_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(FloatObject)

                IntObject* int_other = nullptr;
                FloatObject* float_other = nullptr;
                if (args[1]->is_instance(IntType::get_instance()))
                {
                    int_other = static_cast<IntObject*>(args[1]);
                }
                else if (args[1]->is_instance(FloatType::get_instance()))
                {
                    float_other = static_cast<FloatObject*>(args[1]);
                }
                else
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, 
                        std::format("Unsupport operand type(s) for *: 'float' and '{}'", args[1]->type()->name));
                }

                FloatObject* result_obj = XyA_Allocate_(FloatObject);
                result_obj->value = self->value / ((int_other != nullptr) ? int_other->value : float_other->value);
                return result_obj;
            }

            Object* float_object_equal_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(FloatObject)

                IntObject* int_other = nullptr;
                FloatObject* float_other = nullptr;
                if (args[1]->is_instance(IntType::get_instance()))
                {
                    int_other = static_cast<IntObject*>(args[1]);
                }
                else if (args[1]->is_instance(FloatType::get_instance()))
                {
                    float_other = static_cast<FloatObject*>(args[1]);
                }
                else
                {
                    return BoolObject::get_instance(false);
                }

                return BoolObject::get_instance(self->value == ((int_other != nullptr) ? int_other->value : float_other->value));
            }

            Object* float_object_str_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                XyA_Builtin_Method_Get_Self(FloatObject)

                StringObject* str = XyA_Allocate_(StringObject);                
                str->value = std::to_string(self->value);

                return str;
            }

            Object* float_object_bool_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                XyA_Builtin_Method_Get_Self(FloatObject)

                return BoolObject::get_instance(self->value != 0);
            }

            Object* float_object_greater_than_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(FloatObject)

                IntObject* int_other = nullptr;
                FloatObject* float_other = nullptr;
                if (args[1]->is_instance(IntType::get_instance()))
                {
                    int_other = static_cast<IntObject*>(args[1]);
                }
                else if (args[1]->is_instance(FloatType::get_instance()))
                {
                    float_other = static_cast<FloatObject*>(args[1]);
                }
                else
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, 
                        std::format("Unsupport operand type(s) for >: 'float' and '{}'", args[1]->type()->name));
                }

                return BoolObject::get_instance(self->value > ((int_other != nullptr) ? int_other->value : float_other->value));
            }

            Object* float_object_less_than_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(FloatObject)

                IntObject* int_other = nullptr;
                FloatObject* float_other = nullptr;
                if (args[1]->is_instance(IntType::get_instance()))
                {
                    int_other = static_cast<IntObject*>(args[1]);
                }
                else if (args[1]->is_instance(FloatType::get_instance()))
                {
                    float_other = static_cast<FloatObject*>(args[1]);
                }
                else
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, 
                        std::format("Unsupport operand type(s) for <: 'float' and '{}'", args[1]->type()->name));
                }

                return BoolObject::get_instance(self->value < ((int_other != nullptr) ? int_other->value : float_other->value));  
            }

            Object* float_object_greater_equal_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(FloatObject)

                IntObject* int_other = nullptr;
                FloatObject* float_other = nullptr;
                if (args[1]->is_instance(IntType::get_instance()))
                {
                    int_other = static_cast<IntObject*>(args[1]);
                }
                else if (args[1]->is_instance(FloatType::get_instance()))
                {
                    float_other = static_cast<FloatObject*>(args[1]);
                }
                else
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, 
                        std::format("Unsupport operand type(s) for >=: 'float' and '{}'", args[1]->type()->name));
                }

                return BoolObject::get_instance(self->value >= ((int_other != nullptr) ? int_other->value : float_other->value));
            }

            Object* float_object_less_equal_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(FloatObject)

                IntObject* int_other = nullptr;
                FloatObject* float_other = nullptr;
                if (args[1]->is_instance(IntType::get_instance()))
                {
                    int_other = static_cast<IntObject*>(args[1]);
                }
                else if (args[1]->is_instance(FloatType::get_instance()))
                {
                    float_other = static_cast<FloatObject*>(args[1]);
                }
                else
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, 
                        std::format("Unsupport operand type(s) for <=: 'float' and '{}'", args[1]->type()->name));
                }

                return BoolObject::get_instance(self->value <= ((int_other != nullptr) ? int_other->value : float_other->value));  
            }

            Object* float_object_as_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(FloatObject)
                
                if (!args[1]->is_instance(Type::get_instance()))
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, "The argument 1 is not type");
                }

                Type* target_type = static_cast<Type*>(args[1]);
                if (target_type == FloatType::get_instance())
                {
                    return self;
                }
                if (target_type == IntType::get_instance())
                {
                    IntObject* int_object = XyA_Allocate(IntObject, self->value);
                    return int_object;
                }
                if (target_type == StringType::get_instance())
                {
                    StringObject* string_object = XyA_Allocate(StringObject, std::to_string(self->value));
                    return string_object;
                }

                exception_thrown = true;
                return XyA_Allocate(BuiltinException, std::format("Unsupported type-conversion ('float' to '{}')", target_type->name));
            }
            
        }
    }
}