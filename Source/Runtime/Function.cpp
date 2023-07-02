#pragma once
#include <Runtime/Function.h>
#include <Runtime/Builtin/BuiltinException.h>
#include <Runtime/VirtualMachine.h>
#include <Runtime/Builtin/Null.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        Function::Function()
        {
            this->code_object = XyA_Allocate_(CodeObject);
        }

        Function::~Function()
        {
            XyA_Deallocate(this->code_object);
        }

        Object* Function::call(Object** args, size_t arg_num, bool& exception_thrown) const
        {
            XyA_Function_Check_Arg_Num(this->expected_arg_num)

            Context* function_context = new Context(this->code_object);
            function_context->back = VirtualMachine::get_instance()->cur_context;
            VirtualMachine::get_instance()->cur_context = function_context;

            for (size_t i = 0; i < arg_num; i ++)
            {
                function_context->local_variables[i] = args[i];
                args[i]->reference();
            }
            VirtualMachine::get_instance()->execute_context();

            if (function_context->returned_obj == nullptr)
            {
                function_context->returned_obj = XyA_Allocate_(Builtin::NullObject);
            }
            else
            {
                function_context->returned_obj->ref_count --;  // 不能使用dereference()，防止返回值被释放
            }

            return function_context->returned_obj;
        }

        #ifdef Debug_Display_Object
        std::string Function::to_string() const
        {
            return "<Funtion Object at " + std::to_string((size_t)this) + ">";
        }
        #endif
    }
}