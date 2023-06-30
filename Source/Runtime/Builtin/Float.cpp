#pragma once
#include <Runtime/Builtin/Float.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            FloatType::FloatType()
            {
                this->name = "float";
                this->type = nullptr;
                this->attrs[MagicMethodNames::add_method_name] = XyA_Allocate(BuiltinFunction, float_object_add);
                this->attrs[MagicMethodNames::subtract_method_name] = XyA_Allocate(BuiltinFunction, float_object_subtract);
                this->attrs[MagicMethodNames::multiply_method_name] = XyA_Allocate(BuiltinFunction, float_object_multiply);
                this->attrs[MagicMethodNames::divide_method_name] = XyA_Allocate(BuiltinFunction, float_object_divide);
                this->attrs[MagicMethodNames::equal_method_name] = XyA_Allocate(BuiltinFunction, float_object_equal);
                this->attrs[MagicMethodNames::str_method_name] = XyA_Allocate(BuiltinFunction, float_object_str);
                this->attrs[MagicMethodNames::bool_method_name] = XyA_Allocate(BuiltinFunction, float_object_bool);
                this->attrs[MagicMethodNames::greater_method_name] = XyA_Allocate(BuiltinFunction, float_object_compare_if_greater);
                this->attrs[MagicMethodNames::greater_equal_method_name] = XyA_Allocate(BuiltinFunction, float_object_compare_if_greater_equal);
                this->attrs[MagicMethodNames::less_method_name] = XyA_Allocate(BuiltinFunction, float_object_compare_if_less);
                this->attrs[MagicMethodNames::less_equal_method_name] = XyA_Allocate(BuiltinFunction, float_object_compare_if_less_equal);

                for (const auto& iter : this->attrs)
                {
                    iter.second->reference();
                }
            }

            FloatType* FloatType::get_instance()
            {
                static FloatType instance;
                return &instance;
            }

            FloatObject::FloatObject()
            {
                this->type = FloatType::get_instance();
            }

            /* Float Methods */
            Object* float_object_add(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Builtin_Function_Check_Arg_Num(2)
                XyA_Builtin_Function_Get_Self(FloatObject)
                
                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }
                FloatObject* result_obj = XyA_Allocate_(FloatObject);
                result_obj->value = self->value + other->value;
                return result_obj;
            }

            Object* float_object_subtract(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Builtin_Function_Check_Arg_Num(2)
                XyA_Builtin_Function_Get_Self(FloatObject)

                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }
                FloatObject* result_obj = XyA_Allocate_(FloatObject);
                result_obj->value = self->value - other->value;
                return result_obj;
            }

            Object* float_object_multiply(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Builtin_Function_Check_Arg_Num(2)
                XyA_Builtin_Function_Get_Self(FloatObject)

                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }
                FloatObject* result_obj = XyA_Allocate_(FloatObject);
                result_obj->value = self->value * other->value;
                return result_obj;
            }

            Object* float_object_divide(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Builtin_Function_Check_Arg_Num(2)
                XyA_Builtin_Function_Get_Self(FloatObject)

                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }
                if (other->value == 0)
                {
                    exception_thrown = true;
                    return new BuiltinException("Division by zero");
                }
                FloatObject* result_obj = XyA_Allocate_(FloatObject);
                result_obj->value = self->value / other->value;
                return result_obj;
            }

            Object* float_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Builtin_Function_Check_Arg_Num(2)
                XyA_Builtin_Function_Get_Self(FloatObject)

                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (other == nullptr)
                {
                    return XyA_Allocate(BoolObject, false);
                }
                return XyA_Allocate(BoolObject, self->value == other->value);
            }

            Object* float_object_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Builtin_Function_Check_Arg_Num(1)
                XyA_Builtin_Function_Get_Self(FloatObject)

                StringObject* str = XyA_Allocate_(StringObject);

                std::stringstream ss;
                ss.precision(20);
                ss << self->value;
                str->value = ss.str();

                return str;
            }

            Object* float_object_bool(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Builtin_Function_Check_Arg_Num(1)
                XyA_Builtin_Function_Get_Self(FloatObject)

                return XyA_Allocate(BoolObject, self->value != 0);
            }

            Object* float_object_compare_if_greater(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Builtin_Function_Check_Arg_Num(2)
                XyA_Builtin_Function_Get_Self(FloatObject)

                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Types of arguments must be float, but not " + args[1]->type->name);
                }
                return XyA_Allocate(BoolObject, self->value > other->value);
            }

            Object* float_object_compare_if_less(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Builtin_Function_Check_Arg_Num(2)
                XyA_Builtin_Function_Get_Self(FloatObject)

                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Types of arguments must be int, but not " + args[1]->type->name);
                }
                return XyA_Allocate(BoolObject, self->value < other->value);
            }

            Object* float_object_compare_if_greater_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Builtin_Function_Check_Arg_Num(2)
                XyA_Builtin_Function_Get_Self(FloatObject)
                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Types of arguments must be int, but not " + args[1]->type->name);
                }
                return XyA_Allocate(BoolObject, self->value >= other->value);
            }

            Object* float_object_compare_if_less_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Builtin_Function_Check_Arg_Num(2)
                XyA_Builtin_Function_Get_Self(FloatObject)
                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Types of arguments must be int, but not " + args[1]->type->name);
                }
                return XyA_Allocate(BoolObject, self->value <= other->value);
            }
        }
    }
}