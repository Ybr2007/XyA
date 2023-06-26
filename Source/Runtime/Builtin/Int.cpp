#pragma once
#include <Runtime/Builtin/Int.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            IntType::IntType()
            {
                this->name = "int";
                this->type = nullptr;
                this->attrs[MagicMethodNames::add_method_name] = new BuiltinFunction(int_object_add);
                this->attrs[MagicMethodNames::subtract_method_name] = new BuiltinFunction(int_object_subtract);
                this->attrs[MagicMethodNames::multiply_method_name] = new BuiltinFunction(int_object_multiply);
                this->attrs[MagicMethodNames::divide_method_name] = new BuiltinFunction(int_object_divide);
                this->attrs[MagicMethodNames::equal_method_name] = new BuiltinFunction(int_object_equal);
                this->attrs[MagicMethodNames::str_method_name] = new BuiltinFunction(int_object_str);
                this->attrs[MagicMethodNames::bool_method_name] = new BuiltinFunction(int_object_bool);
                this->attrs[MagicMethodNames::greater_method_name] = new BuiltinFunction(int_object_compare_if_greater);
                this->attrs[MagicMethodNames::greater_equal_method_name] = new BuiltinFunction(int_object_compare_if_greater_equal);
                this->attrs[MagicMethodNames::less_method_name] = new BuiltinFunction(int_object_compare_if_less);
                this->attrs[MagicMethodNames::less_equal_method_name] = new BuiltinFunction(int_object_compare_if_less_equal);
            }

            IntType* IntType::get_instance()
            {
                static IntType instance;
                return &instance;
            }

            IntType* int_type_instance = new IntType;

            IntObject::IntObject()
            {
                this->type = int_type_instance;
            }

            /* Int Methods */
            Object* int_object_add(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                IntObject* self = dynamic_cast<IntObject*>(args[0]);

                IntObject* int_other = dynamic_cast<IntObject*>(args[1]);
                FloatObject* float_other = nullptr;
                if (int_other == nullptr)
                {
                    float_other = dynamic_cast<FloatObject*>(args[1]);
                }

                if (self == nullptr || (int_other == nullptr && float_other == nullptr))
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }

                if (int_other != nullptr)
                {
                    IntObject* result_obj = new IntObject;
                    result_obj->value = self->value + int_other->value;
                    return result_obj;
                }
                else  // float_other != nullptr
                {
                    FloatObject* result_obj = new FloatObject;
                    result_obj->value = self->value + float_other->value;
                    return result_obj;
                }   
            }

            Object* int_object_subtract(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                IntObject* self = dynamic_cast<IntObject*>(args[0]);

                IntObject* int_other = dynamic_cast<IntObject*>(args[1]);
                FloatObject* float_other = nullptr;
                if (int_other == nullptr)
                {
                    float_other = dynamic_cast<FloatObject*>(args[1]);
                }

                if (self == nullptr || (int_other == nullptr && float_other == nullptr))
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }

                if (int_other != nullptr)
                {
                    IntObject* result_obj = new IntObject;
                    result_obj->value = self->value - int_other->value;
                    return result_obj;
                }
                else  // float_other != nullptr
                {
                    FloatObject* result_obj = new FloatObject;
                    result_obj->value = self->value - float_other->value;
                    return result_obj;
                }                
            }

            Object* int_object_multiply(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                IntObject* self = dynamic_cast<IntObject*>(args[0]);

                IntObject* int_other = dynamic_cast<IntObject*>(args[1]);
                FloatObject* float_other = nullptr;
                if (int_other == nullptr)
                {
                    float_other = dynamic_cast<FloatObject*>(args[1]);
                }

                if (self == nullptr || (int_other == nullptr && float_other == nullptr))
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }

                if (int_other != nullptr)
                {
                    IntObject* result_obj = new IntObject;
                    result_obj->value = self->value * int_other->value;
                    return result_obj;
                }
                else  // float_other != nullptr
                {
                    FloatObject* result_obj = new FloatObject;
                    result_obj->value = self->value * float_other->value;
                    return result_obj;
                }   
            }

            Object* int_object_divide(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                IntObject* self = dynamic_cast<IntObject*>(args[0]);

                IntObject* int_other = dynamic_cast<IntObject*>(args[1]);
                FloatObject* float_other = nullptr;
                if (int_other == nullptr)
                {
                    float_other = dynamic_cast<FloatObject*>(args[1]);
                }

                if (self == nullptr || (int_other == nullptr && float_other == nullptr))
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }
                if ((int_other && int_other->value == 0) || (float_other && float_other->value == 0))
                {
                    exception_thrown = true;
                    return new BuiltinException("Division by zero");
                }

                if (int_other != nullptr)
                {
                    if (self->value % int_other->value == 0)
                    {
                        IntObject* result_obj = new IntObject;
                        result_obj->value = self->value / int_other->value;
                        return result_obj;
                    }
                    else 
                    {
                        FloatObject* result_obj = new FloatObject;
                        result_obj->value = self->value * 1.000000000000000 / int_other->value;
                        return result_obj;
                    }
                }
                else  // float_other != nullptr
                {
                    FloatObject* result_obj = new FloatObject;
                    result_obj->value = self->value / float_other->value;
                    return result_obj;
                }   
            }

            Object* int_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                IntObject* self = dynamic_cast<IntObject*>(args[0]);

                IntObject* int_other = dynamic_cast<IntObject*>(args[1]);
                FloatObject* float_other = nullptr;
                if (int_other == nullptr)
                {
                    float_other = dynamic_cast<FloatObject*>(args[1]);
                }

                if (self == nullptr || (int_other == nullptr && float_other == nullptr))
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }

                if (int_other != nullptr)
                {
                    return new BoolObject(self->value == int_other->value);
                }
                else  // float_other != nullptr
                {
                    return new BoolObject(self->value == float_other->value);
                }   
            }

            Object* int_object_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 1)
                {
                    exception_thrown = true;
                    return new BuiltinException("Excepted 1 argument, got " + std::to_string(arg_num));
                }

                IntObject* self = dynamic_cast<IntObject*>(args[0]);
                if (self == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("The type of argument 'self' must be int." + std::to_string(arg_num));
                }

                StringObject* str = new StringObject;
                str->value = std::to_string(self->value);

                return str;
            }

            Object* int_object_bool(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 1)
                {
                    exception_thrown = true;
                    return new BuiltinException("Excepted 1 argument, got " + std::to_string(arg_num));
                }

                IntObject* self = dynamic_cast<IntObject*>(args[0]);
                if (self == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("The type of argument 'self' must be int." + std::to_string(arg_num));
                }

                return new BoolObject(self->value != 0);
            }

            Object* int_object_compare_if_greater(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                IntObject* self = dynamic_cast<IntObject*>(args[0]);

                IntObject* int_other = dynamic_cast<IntObject*>(args[1]);
                FloatObject* float_other = nullptr;
                if (int_other == nullptr)
                {
                    float_other = dynamic_cast<FloatObject*>(args[1]);
                }

                if (self == nullptr || (int_other == nullptr && float_other == nullptr))
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }

                if (int_other != nullptr)
                {
                    return new BoolObject(self->value > int_other->value);
                }
                else  // float_other != nullptr
                {
                    return new BoolObject(self->value > float_other->value);
                }   
            }

            Object* int_object_compare_if_less(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                IntObject* self = dynamic_cast<IntObject*>(args[0]);

                IntObject* int_other = dynamic_cast<IntObject*>(args[1]);
                FloatObject* float_other = nullptr;
                if (int_other == nullptr)
                {
                    float_other = dynamic_cast<FloatObject*>(args[1]);
                }

                if (self == nullptr || (int_other == nullptr && float_other == nullptr))
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }

                if (int_other != nullptr)
                {
                    return new BoolObject(self->value < int_other->value);
                }
                else  // float_other != nullptr
                {
                    return new BoolObject(self->value < float_other->value);
                }  
            }

            Object* int_object_compare_if_greater_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                IntObject* self = dynamic_cast<IntObject*>(args[0]);

                IntObject* int_other = dynamic_cast<IntObject*>(args[1]);
                FloatObject* float_other = nullptr;
                if (int_other == nullptr)
                {
                    float_other = dynamic_cast<FloatObject*>(args[1]);
                }

                if (self == nullptr || (int_other == nullptr && float_other == nullptr))
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }

                if (int_other != nullptr)
                {
                    return new BoolObject(self->value >= int_other->value);
                }
                else  // float_other != nullptr
                {
                    return new BoolObject(self->value >= float_other->value);
                }  
            }

            Object* int_object_compare_if_less_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                IntObject* self = dynamic_cast<IntObject*>(args[0]);

                IntObject* int_other = dynamic_cast<IntObject*>(args[1]);
                FloatObject* float_other = nullptr;
                if (int_other == nullptr)
                {
                    float_other = dynamic_cast<FloatObject*>(args[1]);
                }

                if (self == nullptr || (int_other == nullptr && float_other == nullptr))
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }

                if (int_other != nullptr)
                {
                    return new BoolObject(self->value < int_other->value);
                }
                else  // float_other != nullptr
                {
                    return new BoolObject(self->value < float_other->value);
                }  
            }
        }  // namespace Builtin
    }  // namespace Runtime
}  // namespace XyA