#pragma once
#include <Runtime/Builtin/Int.h>
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
                this->name = "int";
                this->magic_methods[MagicMethodNames::add_method_index] = XyA_Allocate(BuiltinFunction, int_object_add);
                this->magic_methods[MagicMethodNames::subtract_method_index] = XyA_Allocate(BuiltinFunction, int_object_subtract);
                this->magic_methods[MagicMethodNames::multiply_method_index] = XyA_Allocate(BuiltinFunction, int_object_multiply);
                this->magic_methods[MagicMethodNames::divide_method_index] = XyA_Allocate(BuiltinFunction, int_object_divide);
                this->magic_methods[MagicMethodNames::equal_method_index] = XyA_Allocate(BuiltinFunction, int_object_equal);
                this->magic_methods[MagicMethodNames::str_method_index] = XyA_Allocate(BuiltinFunction, int_object_str);
                this->magic_methods[MagicMethodNames::bool_method_index] = XyA_Allocate(BuiltinFunction, int_object_bool);
                this->magic_methods[MagicMethodNames::greater_method_index] = XyA_Allocate(BuiltinFunction, int_object_compare_if_greater);
                this->magic_methods[MagicMethodNames::greater_equal_method_index] = XyA_Allocate(BuiltinFunction, int_object_compare_if_greater_equal);
                this->magic_methods[MagicMethodNames::less_method_index] = XyA_Allocate(BuiltinFunction, int_object_compare_if_less);
                this->magic_methods[MagicMethodNames::less_equal_method_index] = XyA_Allocate(BuiltinFunction, int_object_compare_if_less_equal);

                this->reference_attrs();
            }

            IntType* IntType::get_instance()
            {
                static IntType instance;
                return &instance;
            }
            IntObject::IntObject()
            {
                this->__type = IntType::get_instance();
            }

            /* Int Methods */
            Object* int_object_add(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Method_Get_Self(IntObject)

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
                    IntObject* result_obj = XyA_Allocate_(IntObject);
                    result_obj->value = self->value + int_other->value;

                    return result_obj;
                }
                else  // float_other != nullptr
                {
                    FloatObject* result_obj = XyA_Allocate_(FloatObject);
                    result_obj->value = self->value + float_other->value;
                    return result_obj;
                }   
            }

            Object* int_object_subtract(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Method_Get_Self(IntObject)

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
                    IntObject* result_obj = XyA_Allocate_(IntObject);
                    result_obj->value = self->value - int_other->value;
                    return result_obj;
                }
                else  // float_other != nullptr
                {
                    FloatObject* result_obj = XyA_Allocate_(FloatObject);
                    result_obj->value = self->value - float_other->value;
                    return result_obj;
                }                
            }

            Object* int_object_multiply(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Method_Get_Self(IntObject)

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
                    IntObject* result_obj = XyA_Allocate_(IntObject);
                    result_obj->value = self->value * int_other->value;
                    return result_obj;
                }
                else  // float_other != nullptr
                {
                    FloatObject* result_obj = XyA_Allocate_(FloatObject);
                    result_obj->value = self->value * float_other->value;
                    return result_obj;
                }   
            }

            Object* int_object_divide(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Method_Get_Self(IntObject)

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
                        IntObject* result_obj = XyA_Allocate_(IntObject);
                        result_obj->value = self->value / int_other->value;
                        return result_obj;
                    }
                    else 
                    {
                        FloatObject* result_obj = XyA_Allocate_(FloatObject);
                        result_obj->value = self->value * 1.000000000000000 / int_other->value;
                        return result_obj;
                    }
                }
                else  // float_other != nullptr
                {
                    FloatObject* result_obj = XyA_Allocate_(FloatObject);
                    result_obj->value = self->value / float_other->value;
                    return result_obj;
                }   
            }

            Object* int_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BoolObject, self->value == int_other->value);
                }
                else  // float_other != nullptr
                {
                    return XyA_Allocate(BoolObject, self->value == float_other->value);
                }   
            }

            Object* int_object_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                XyA_Method_Get_Self(IntObject)

                StringObject* str = XyA_Allocate_(StringObject);                
                str->value = std::to_string(self->value);

                return str;
            }

            Object* int_object_bool(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                XyA_Method_Get_Self(IntObject)

                return XyA_Allocate(BoolObject, self->value != 0);
            }

            Object* int_object_compare_if_greater(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BoolObject, self->value > int_other->value);
                }
                else  // float_other != nullptr
                {
                    return XyA_Allocate(BoolObject, self->value > float_other->value);
                }   
            }

            Object* int_object_compare_if_less(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BoolObject, self->value < int_other->value);
                }
                else  // float_other != nullptr
                {
                    return XyA_Allocate(BoolObject, self->value < float_other->value);
                }  
            }

            Object* int_object_compare_if_greater_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BoolObject, self->value >= int_other->value);
                }
                else  // float_other != nullptr
                {
                    return XyA_Allocate(BoolObject, self->value >= float_other->value);
                }  
            }

            Object* int_object_compare_if_less_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(2)
                XyA_Method_Get_Self(IntObject)

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
                    return XyA_Allocate(BoolObject, self->value <= int_other->value);
                }
                else  // float_other != nullptr
                {
                    return XyA_Allocate(BoolObject, self->value <= float_other->value);
                }  
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