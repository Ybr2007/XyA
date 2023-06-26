#pragma once
#include <Runtime/Builtin/Float.h>


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
                this->attrs[MagicMethodNames::add_method_name] = new BuiltinFunction(float_object_add);
                this->attrs[MagicMethodNames::subtract_method_name] = new BuiltinFunction(float_object_subtract);
                this->attrs[MagicMethodNames::multiply_method_name] = new BuiltinFunction(float_object_multiply);
                this->attrs[MagicMethodNames::divide_method_name] = new BuiltinFunction(float_object_divide);
                this->attrs[MagicMethodNames::equal_method_name] = new BuiltinFunction(float_object_equal);
                this->attrs[MagicMethodNames::str_method_name] = new BuiltinFunction(float_object_str);
                this->attrs[MagicMethodNames::bool_method_name] = new BuiltinFunction(float_object_bool);
                this->attrs[MagicMethodNames::greater_method_name] = new BuiltinFunction(float_object_compare_if_greater);
                this->attrs[MagicMethodNames::greater_equal_method_name] = new BuiltinFunction(float_object_compare_if_greater_equal);
                this->attrs[MagicMethodNames::less_method_name] = new BuiltinFunction(float_object_compare_if_less);
                this->attrs[MagicMethodNames::less_equal_method_name] = new BuiltinFunction(float_object_compare_if_less_equal);
            }

            FloatType* FloatType::get_instance()
            {
                static FloatType* instance = new FloatType;
                return instance;
            }

            FloatObject::FloatObject()
            {
                this->type = FloatType::get_instance();
            }

            /* Float Methods */
            Object* float_object_add(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                FloatObject* self = dynamic_cast<FloatObject*>(args[0]);
                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (self == nullptr || other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }
                FloatObject* result_obj = new FloatObject;
                result_obj->value = self->value + other->value;
                return result_obj;
            }

            Object* float_object_subtract(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                FloatObject* self = dynamic_cast<FloatObject*>(args[0]);
                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (self == nullptr || other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }
                FloatObject* result_obj = new FloatObject;
                result_obj->value = self->value - other->value;
                return result_obj;
            }

            Object* float_object_multiply(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                FloatObject* self = dynamic_cast<FloatObject*>(args[0]);
                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (self == nullptr || other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }
                FloatObject* result_obj = new FloatObject;
                result_obj->value = self->value * other->value;
                return result_obj;
            }

            Object* float_object_divide(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                FloatObject* self = dynamic_cast<FloatObject*>(args[0]);
                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (self == nullptr || other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }
                if (other->value == 0)
                {
                    exception_thrown = true;
                    return new BuiltinException("Division by zero");
                }
                FloatObject* result_obj = new FloatObject;
                result_obj->value = self->value / other->value;
                return result_obj;
            }

            Object* float_object_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                FloatObject* self = dynamic_cast<FloatObject*>(args[0]);
                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (self == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Type Error");
                }
                if (other == nullptr)
                {
                    return new BoolObject(false);
                }
                return new BoolObject(self->value == other->value);
            }

            Object* float_object_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 1)
                {
                    exception_thrown = true;
                    return new BuiltinException("Excepted 1 argument, got " + std::to_string(arg_num));
                }

                FloatObject* self = dynamic_cast<FloatObject*>(args[0]);
                if (self == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("The type of argument 'self' must be int." + std::to_string(arg_num));
                }

                StringObject* str = new StringObject;

                std::stringstream ss;
                ss.precision(20);
                ss << self->value;
                str->value = ss.str();

                return str;
            }

            Object* float_object_bool(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 1)
                {
                    exception_thrown = true;
                    return new BuiltinException("Excepted 1 argument, got " + std::to_string(arg_num));
                }

                FloatObject* self = dynamic_cast<FloatObject*>(args[0]);
                if (self == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("The type of argument 'self' must be int." + std::to_string(arg_num));
                }

                return new BoolObject(self->value != 0);
            }

            Object* float_object_compare_if_greater(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                FloatObject* self = dynamic_cast<FloatObject*>(args[0]);
                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (self == nullptr || other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Types of arguments must be int, but not " + args[1]->type->name);
                }
                return new BoolObject(self->value > other->value);
            }

            Object* float_object_compare_if_less(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                FloatObject* self = dynamic_cast<FloatObject*>(args[0]);
                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (self == nullptr || other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Types of arguments must be int, but not " + args[1]->type->name);
                }
                return new BoolObject(self->value < other->value);
            }

            Object* float_object_compare_if_greater_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                FloatObject* self = dynamic_cast<FloatObject*>(args[0]);
                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (self == nullptr || other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Types of arguments must be int, but not " + args[1]->type->name);
                }
                return new BoolObject(self->value >= other->value);
            }

            Object* float_object_compare_if_less_equal(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 2)
                {
                    exception_thrown = true;
                    return new BuiltinException("Expected 2 arguments, got " + std::to_string(arg_num));
                }
                FloatObject* self = dynamic_cast<FloatObject*>(args[0]);
                FloatObject* other = dynamic_cast<FloatObject*>(args[1]);
                if (self == nullptr || other == nullptr)
                {
                    exception_thrown = true;
                    return new BuiltinException("Types of arguments must be int, but not " + args[1]->type->name);
                }
                return new BoolObject(self->value <= other->value);
            }
        }
    }
}