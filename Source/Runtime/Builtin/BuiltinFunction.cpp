#pragma once
#include <Runtime/Builtin/BuiltinFunction.h>
#include <Runtime/Builtin/String.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            BuiltinFunctionType::BuiltinFunctionType()
            {
                this->name = "builtin function";
                this->type = nullptr;
                this->attrs[MagicMethodNames::str_method_name] = new Builtin::BuiltinFunction(builtin_function_str);
            }
            
            Object* builtin_function_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                StringObject* str = new StringObject;
                str->value = "<builtin-function>";
                return str;
            }

            BuiltinFunctionType* builtin_function_type_instance = new BuiltinFunctionType;

            BuiltinFunction::BuiltinFunction(std::function<Object*(Object**, size_t, bool&)> cpp_function)
            {
                this->type = builtin_function_type_instance;
                this->cpp_function = cpp_function;
            }

            Object* BuiltinFunction::call(Object** args, size_t arg_num, bool& exception_thrown) const
            {
                return this->cpp_function(args, arg_num, exception_thrown);
            }
        }
    }
}