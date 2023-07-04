#pragma once
#include <Runtime/Object.h>
#include <Runtime/CodeObject.h>


namespace XyA
{
    namespace Runtime
    {
        class FunctionType : public Type
        {
        public:
            FunctionType();

            static FunctionType* get_instance();
        };

        class Function : public BaseFunction
        {
        public:
            CodeObject* code_object;
            size_t expected_arg_num;

            Function();
            ~Function();
            virtual Object* call(Object** args, size_t arg_num, bool& exception_thrown) const; 
            
            #ifdef Debug_Display_Object
            virtual std::string to_string() const;
            #endif
        };
    }
}