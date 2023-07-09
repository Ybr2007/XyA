#pragma once
#include <Runtime/Builtin/BuiltinFunction.h>
#include <Runtime/Builtin/String.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            BuiltinFunctionType builtin_function_type_instance;

            BuiltinFunctionType::BuiltinFunctionType()
            {
                this->name = "builtin function";
                this->attrs[MagicMethodNames::str_method_name] = XyA_Allocate(BuiltinFunction, builtin_function_str);
                
                this->reference_attrs();
            }

            BuiltinFunctionType* BuiltinFunctionType::get_instance()
            {
                return &builtin_function_type_instance;
            }
            
            Object* builtin_function_str(Object** args, size_t arg_num, bool& exception_thrown)
            {
                StringObject* str = XyA_Allocate_(StringObject);
                str->value = "<builtin-function>";
                return str;
            }

            BuiltinFunction::BuiltinFunction(std::function<Object*(Object**, size_t, bool&)> cpp_function)
            {
                this->__type = BuiltinFunctionType::get_instance();
                this->cpp_function = cpp_function;
            }

            Object* BuiltinFunction::call(Object** args, size_t arg_num, bool& exception_thrown) const
            {
                for (size_t i = 0; i < arg_num; i ++)
                {
                    args[i]->reference();
                }

                Object* result = this->cpp_function(args, arg_num, exception_thrown);

                for (size_t i = 0; i < arg_num; i ++)
                {
                    args[i]->dereference();
                }
                
                return result;
            }
        }
    }
}