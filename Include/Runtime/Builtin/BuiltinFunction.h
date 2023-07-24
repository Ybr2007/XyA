#pragma once
#include <Runtime/BasicObjectDefs.h>
#include <Runtime/MagicMethodNames.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {    
            #define XyA_Builtin_Method_Get_Self(T) \
                if (!args[0]->is_instance(T::static_type())) \
                {   \
                    exception_thrown = true; \
                    return XyA_Allocate(BuiltinException, "Type Error"); \
                } \
                T* self = static_cast<T*>(args[0]);
                
            Object* builtin_function_str(Object** args, size_t arg_num, bool& exception_thrown);  

            class BuiltinFunctionType : public Type
            {
            public:
                BuiltinFunctionType();

                static BuiltinFunctionType* get_instance();
            };

            using builtin_cpp_function_ptr = Object*(*)(Object**, size_t, bool&);

            class BuiltinFunction : public BaseFunction
            {
            public:
                builtin_cpp_function_ptr cpp_function;

                BuiltinFunction(builtin_cpp_function_ptr);
                virtual Object* call(Object** args, size_t arg_num, bool& exception_thrown) const;
            };
        }
    }
}