#include <Runtime/Builtin/Int.h>

#include <format>
#include <Runtime/Builtin/BuiltinFunction.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            IntType::IntType()
            {
                Type("int");
                this->ref_count_enabled = false;
                this->set_attr(MagicMethodNames::add_method_name_id, XyA_Allocate(BuiltinFunction, int_object_add_method));
                this->set_attr(MagicMethodNames::subtract_method_name_id, XyA_Allocate(BuiltinFunction, int_object_sub_method));
                this->set_attr(MagicMethodNames::multiply_method_name_id, XyA_Allocate(BuiltinFunction, int_object_mul_method));
                this->set_attr(MagicMethodNames::divide_method_name_id, XyA_Allocate(BuiltinFunction, int_object_divide));
                this->set_attr(MagicMethodNames::equal_method_name_id, XyA_Allocate(BuiltinFunction, int_object_equal));
                this->set_attr(MagicMethodNames::str_method_name_id, XyA_Allocate(BuiltinFunction, int_object_str_method));
                this->set_attr(MagicMethodNames::bool_method_name_id, XyA_Allocate(BuiltinFunction, int_object_bool_method));
                this->set_attr(MagicMethodNames::greater_method_name_id, XyA_Allocate(BuiltinFunction, int_object_greater_than_method));
                this->set_attr(MagicMethodNames::greater_equal_method_name_id, XyA_Allocate(BuiltinFunction, int_object_greater_equal_method));
                this->set_attr(MagicMethodNames::less_method_name_id, XyA_Allocate(BuiltinFunction, int_object_less_than_method));
                this->set_attr(MagicMethodNames::less_equal_method_name_id, XyA_Allocate(BuiltinFunction, int_object_less_equal_method));
                this->set_attr(MagicMethodNames::as_method_name_id, XyA_Allocate(BuiltinFunction, int_object_as_method));
            }

            IntType* IntType::get_instance()
            {
                static IntType instance;
                return &instance;
            }

            IntObject::IntObject() : value(0)
            {
                this->__type = IntType::get_instance();
            }

            IntObject::IntObject(long long value_) : value(value_)
            {
                this->__type = IntType::get_instance();
            }

            IntType* IntObject::static_type()
            {
                return IntType::get_instance();
            }

            /* Int Methods */
            Object* int_object_add_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BuiltinException, "Type Error");
                }

                if (int_other != nullptr)
                {
                    IntObject* result_obj = XyA_Allocate(IntObject, self->value + int_other->value);

                    return result_obj;
                }
                else  // float_other != nullptr
                {
                    FloatObject* result_obj = XyA_Allocate(FloatObject, self->value + float_other->value);
                    return result_obj;
                }   
            }

            Object* int_object_sub_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BuiltinException, "Type Error");
                }

                if (int_other != nullptr)
                {
                    IntObject* result_obj = XyA_Allocate(IntObject, self->value - int_other->value);
                    return result_obj;
                }
                else  // float_other != nullptr
                {
                    FloatObject* result_obj = XyA_Allocate(FloatObject, self->value - float_other->value);
                    return result_obj;
                }                
            }

            Object* int_object_mul_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BuiltinException, "Type Error");
                }

                if (int_other != nullptr)
                {
                    IntObject* result_obj = XyA_Allocate(IntObject, self->value * int_other->value);
                    return result_obj;
                }
                else  // float_other != nullptr
                {
                    FloatObject* result_obj = XyA_Allocate(FloatObject, self->value * float_other->value);
                    return result_obj;
                }   
            }

            Object* int_object_divide(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BuiltinException, "Type Error");
                }

                if ((int_other && int_other->value == 0) || (float_other && float_other->value == 0))
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, "Division by zero");
                }

                if (int_other != nullptr)
                {
                    if (self->value % int_other->value == 0)
                    {
                        IntObject* result_obj = XyA_Allocate(IntObject, self->value / int_other->value);
                        return result_obj;
                    }
                    else 
                    {
                        FloatObject* result_obj = XyA_Allocate(
                            FloatObject, self->value * 1.000000000000000 / int_other->value);
                        return result_obj;
                    }
                }
                else  // float_other != nullptr
                {
                    FloatObject* result_obj = XyA_Allocate(FloatObject, self->value / float_other->value);
                    return result_obj;
                }   
            }

            Object* int_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BuiltinException, "Type Error");
                }

                if (int_other != nullptr)
                {
                    return BoolObject::get_instance(self->value == int_other->value);
                }
                else  // float_other != nullptr
                {
                    return BoolObject::get_instance(self->value == float_other->value);
                }   
            }

            Object* int_object_str_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                XyA_Builtin_Method_Get_Self(IntObject)

                StringObject* str = XyA_Allocate(StringObject, std::to_string(self->value));

                return str;
            }

            Object* int_object_bool_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                XyA_Builtin_Method_Get_Self(IntObject)

                return BoolObject::get_instance(self->value != 0);
            }

            Object* int_object_greater_than_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BuiltinException, "Type Error");
                }

                if (int_other != nullptr)
                {
                    return BoolObject::get_instance(self->value > int_other->value);
                }
                else  // float_other != nullptr
                {
                    return BoolObject::get_instance(self->value > float_other->value);
                }   
            }

            Object* int_object_less_than_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BuiltinException, "Type Error");
                }

                if (int_other != nullptr)
                {
                    return BoolObject::get_instance(self->value < int_other->value);
                }
                else  // float_other != nullptr
                {
                    return BoolObject::get_instance(self->value < float_other->value);
                }  
            }

            Object* int_object_greater_equal_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BuiltinException, "Type Error");
                }

                if (int_other != nullptr)
                {
                    return BoolObject::get_instance(self->value >= int_other->value);
                }
                else  // float_other != nullptr
                {
                    return BoolObject::get_instance(self->value >= float_other->value);
                }  
            }

            Object* int_object_less_equal_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BuiltinException, "Type Error");
                }

                if (int_other != nullptr)
                {
                    return BoolObject::get_instance(self->value <= int_other->value);
                }
                else  // float_other != nullptr
                {
                    return BoolObject::get_instance(self->value <= float_other->value);
                }  
            }

            Object* int_object_as_method(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Builtin_Method_Get_Self(IntObject)
                
                if (!args[1]->is_instance(Type::get_instance()))
                {
                    exception_thrown = true;
                    return XyA_Allocate(BuiltinException, "The argument 1 is not type");
                }

                Type* target_type = static_cast<Type*>(args[1]);
                if (target_type == IntType::get_instance())
                {
                    return self;
                }
                if (target_type == StringType::get_instance())
                {
                    StringObject* string_object = XyA_Allocate(StringObject, std::to_string(self->value));
                    return string_object;
                }

                exception_thrown = true;
                return XyA_Allocate(BuiltinException, std::format("Unsupported type-conversion ('int' to '{}')", target_type->name));
            }

            #ifdef Debug_Display_Object
            std::string IntObject::to_string() const
            {
                return "<Int Object: " + std::to_string(this->value) + " at " + std::to_string((size_t)this) + ">";
            }
            #endif
        }  // namespace Builtin
    }  // namespace Runtime
}  // namespace XyA