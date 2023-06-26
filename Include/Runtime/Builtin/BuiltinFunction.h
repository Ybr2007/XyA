#pragma once
#include <functional>
#include <Runtime/Object.h>
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

            class BuiltinFunction : public BaseFunction
            {
            public:
                std::function<Object*(Object**, size_t, bool&)> cpp_function;

                BuiltinFunction(std::function<Object*(Object**, size_t, bool&)>);
                virtual Object* call(Object** args, size_t arg_num, bool& exception_thrown) const;
            };

        }
    }
}