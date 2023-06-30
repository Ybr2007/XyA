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
            #define XyA_Builtin_Function_Check_Arg_Num(expected_arg_num) \
                if (arg_num != expected_arg_num) \
                { \
                    exception_thrown = true; \
                    return new BuiltinException("Expected" + std::string(#expected_arg_num) + "arguments, got " + std::to_string(arg_num)); \
                }

            #define XyA_Builtin_Function_Get_Self(T) \
                T* self = dynamic_cast<T*>(args[0]); \
                if (self == nullptr) \
                { \
                    exception_thrown = true; \
                    return new BuiltinException("Type Error"); \
                }
                
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