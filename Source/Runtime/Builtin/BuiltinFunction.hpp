#pragma once
#include <functional>
#include <Runtime/Object.hpp>
#include <Runtime/MagicMethodNames.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            // 实现于Runtime/Builtin/String.hpp
            Object* builtin_function_str(Object** args, size_t arg_num, bool& exception_thrown);  


            class BuiltinFunctionType : public Type
            {
            public:
                BuiltinFunctionType();
            };

            BuiltinFunctionType* builtin_function_type_instance = new BuiltinFunctionType;

            class BuiltinFunction : public BaseFunction
            {
            public:
                std::function<Object*(Object**, size_t, bool&)> cpp_function;

                BuiltinFunction(std::function<Object*(Object**, size_t, bool&)>);
                virtual Object* call(Object** args, size_t arg_num, bool& exception_thrown) const;
            };

            BuiltinFunctionType::BuiltinFunctionType()
            {
                this->name = "builtin function";
                this->attrs[MagicMethodNames::str_method_name] = new Builtin::BuiltinFunction(builtin_function_str);
            }

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