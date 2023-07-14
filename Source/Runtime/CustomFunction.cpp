#pragma once
#include <Runtime/CustomFunction.h>
#include <Runtime/Builtin/BuiltinException.h>
#include <Runtime/VirtualMachine.h>
#include <Runtime/Builtin/Null.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        FunctionType::FunctionType()
        {
            this->name = "function";
        }

        FunctionType* FunctionType::get_instance()
        {
            static FunctionType instance;
            return &instance;
        }

        CustomFunction::CustomFunction()
        {
            this->__type = FunctionType::get_instance();
            this->code_object = XyA_Allocate_(CodeObject);
        }

        CustomFunction::~CustomFunction()
        {
            XyA_Deallocate(this->code_object);
        }

        Object* CustomFunction::call(Object** args, size_t arg_num, bool& exception_thrown) const
        {
            XyA_Function_Check_Arg_Num(this->expected_arg_num)

            Context* function_context = XyA_Allocate(Context, this->code_object);
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
                function_context->returned_obj = Builtin::NullObject::get_instance();
            }
            else
            {
                function_context->returned_obj->ref_count --;  // 不能使用dereference()，防止返回值被释放
            }

            return function_context->returned_obj;
        }

        #ifdef Debug_Display_Object
        std::string CustomFunction::to_string() const
        {
            return "<Funtion Object at " + std::to_string((size_t)this) + ">";
        }
        #endif
    }
}