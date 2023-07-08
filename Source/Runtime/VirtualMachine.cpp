#pragma once
#include <Runtime/MemoryManager.hpp>
#include <Runtime/VirtualMachine.h>
#include <format>


namespace XyA
{
    namespace Runtime
    {
        VirtualMachine* VirtualMachine::get_instance()
        {
            static VirtualMachine instance;
            return &instance;
        }

        void VirtualMachine::execute(Context* global_context)
        {
            this->global_context = global_context;
            this->cur_context = global_context;
            CodeObject* global_code_object = this->global_context->code_obj;

            this->__init_global_context();
            this->execute_context();

            XyA_Deallocate(global_code_object);
        }

        void VirtualMachine::execute_context()
        {
            while (this->cur_context->instruction_ptr < this->cur_context->code_obj->instructions.size())
            {
                Instruction* cur_instruction = this->cur_context->cur_instruction();
                // printf("Instruction %d, %s\n", (size_t)VirtualMachine::get_instance()->cur_context, cur_instruction->to_string().c_str());
                this->__excute_instruction(cur_instruction);
                // printf("FINISH\n");
                this->cur_context->instruction_ptr ++;
            }
            this->__back_context();
        }

        void VirtualMachine::__init_global_context()
        {
            Object*& builtin_print_function = this->global_context->local_variables[0]; // 0: global_context->code_obj->variable_name_indices["print"]
            builtin_print_function = XyA_Allocate(Builtin::BuiltinFunction, Builtin::print);
            builtin_print_function->reference();

            Object*& builtin__get_ref_count_function = this->global_context->local_variables[1]; // 1: global_context->code_obj->variable_name_indices["_get_ref_count"]
            builtin__get_ref_count_function = XyA_Allocate(Builtin::BuiltinFunction, Builtin::_get_ref_count);
            builtin__get_ref_count_function->reference();

            Object*& builtin__get_id_function = this->global_context->local_variables[2];  // 2: global_context->code_obj->variable_name_indices["_get_id"]
            builtin__get_id_function = XyA_Allocate(Builtin::BuiltinFunction, Builtin::_get_id);
            builtin__get_id_function->reference();

            Object*& builtin_clock_function = this->global_context->local_variables[3];  // 3: global_context->code_obj->variable_name_indices["clock"]
            builtin_clock_function = XyA_Allocate(Builtin::BuiltinFunction, Builtin::clock_);
            builtin_clock_function->reference();

            Object*& builtin_sizeof_function = this->global_context->local_variables[4];  // 4: global_context->code_obj->variable_name_indices["clock"]
            builtin_sizeof_function = XyA_Allocate(Builtin::BuiltinFunction, Builtin::sizeof_);
            builtin_sizeof_function->reference();
        }

        void VirtualMachine::__excute_instruction(Instruction* instruction)
        {
            switch (instruction->type)
            {
            case InstructionType::LoadLiteral:
            {
                Object* literal = this->cur_context->get_literal_obj_at(instruction->parameter);
                this->cur_context->push_operand(literal);
                break;
            }

            case InstructionType::LoadVariable:
            {
                Object* variable = this->cur_context->get_variable_at(instruction->parameter);
                if (variable == nullptr)
                {
                    std::string variable_name = this->cur_context->get_variable_name(instruction->parameter);

                    this->__throw_exception("The variable '" + variable_name + "' is not defined or already deleted.");
                }
                this->cur_context->push_operand(variable);
                break;
            }

            case InstructionType::LoadGlobal:
            {
                Object* variable = this->global_context->get_variable_at(instruction->parameter);
                if (variable == nullptr)
                {
                    std::string variable_name = this->global_context->get_variable_name(instruction->parameter);
                    this->__throw_exception("The variable '" + variable_name + "' is not defined or already deleted.");
                }
                this->cur_context->push_operand(variable);
                break;
            }

            case InstructionType::GetAttr:
            {
                Object* attr_owner = this->cur_context->top_operand();

                Object* attr;
                TryGetAttrResult operation_result = attr_owner->try_get_attr(
                    this->cur_context->code_obj->names[instruction->parameter], attr);

                if (operation_result == TryGetAttrResult::NotFound)
                {
                    this->__throw_exception("The variable '" + this->cur_context->code_obj->names[instruction->parameter] + 
                        "' is not defined or already deleted.");
                }
                this->cur_context->set_top_operand(attr);

                break;
            }

            case InstructionType::GetMethod:
            {
                Object* attr_owner = this->cur_context->top_operand();

                Object* attr;
                TryGetAttrResult operation_result = attr_owner->try_get_attr(
                    this->cur_context->code_obj->names[instruction->parameter], attr);

                if (operation_result == TryGetAttrResult::NotFound)
                {
                    this->__throw_exception("The variable '" + this->cur_context->code_obj->names[instruction->parameter] + 
                        "' is not defined or already deleted.");
                }
                this->cur_context->push_operand(attr);

                break;
            }

            case InstructionType::StroeVariable:
            {
                Object* old_variable = this->cur_context->local_variables[instruction->parameter];
                if (old_variable != nullptr)
                {
                    old_variable->dereference();
                }

                this->cur_context->local_variables[instruction->parameter] = this->cur_context->pop_operand();
                this->cur_context->local_variables[instruction->parameter]->reference();

                break;
            }

            case InstructionType::StoreAttr:
            {
                Object* new_attr = this->cur_context->pop_operand();
                Object* attr_owner = this->cur_context->pop_operand();
                
                Object* old_attr;
                TryGetAttrResult operation_result = attr_owner->try_get_attr(
                    this->cur_context->code_obj->names[instruction->parameter], old_attr);

                if (operation_result == TryGetAttrResult::OK)
                {
                    old_attr->dereference();
                }
                else if (!attr_owner->type()->instance_allow_external_attr)
                {
                    this->__throw_exception(
                        std::format("Variables of type '{}' do not allow external attribute addition", attr_owner->type()->name));
                }

                attr_owner->attrs[this->cur_context->code_obj->names[instruction->parameter]] = new_attr;
                new_attr->reference();

                break;
            }

            case InstructionType::BinaryAdd:
            {
                this->__call_binary_operation_magic_method(MagicMethodNames::add_method_name);
                break;
            }

            case InstructionType::BinarySubtract:
            {
                this->__call_binary_operation_magic_method(MagicMethodNames::subtract_method_name);
                break;
            }

            case InstructionType::BinaryMultiply:
            {
                this->__call_binary_operation_magic_method(MagicMethodNames::multiply_method_name);
                break;
            }

            case InstructionType::BinaryDevide:
            {
                this->__call_binary_operation_magic_method(MagicMethodNames::divide_method_name);
                break;
            }

            case InstructionType::CompareIfEqual:
            {
                this->__call_compare_magic_method(MagicMethodNames::equal_method_name);
                break;
            }

            case InstructionType::CompareIfGreaterThan:
            {
                this->__call_compare_magic_method(MagicMethodNames::greater_method_name);
                break;
            }

            case InstructionType::CompareIfGreaterEqual:
            {
                this->__call_compare_magic_method(MagicMethodNames::greater_equal_method_name);
                break;
            }

            case InstructionType::CompareIfLessThan:
            {
                this->__call_compare_magic_method(MagicMethodNames::less_method_name);
                break;
            }

            case InstructionType::CompareIfLessEqual:
            {
                this->__call_compare_magic_method(MagicMethodNames::less_equal_method_name);
                break;
            }

            case InstructionType::CompareIfNotEqual:
            {
                this->__call_compare_magic_method(MagicMethodNames::not_equal_method_name);
                break;
            }

            case InstructionType::PopJumpIfFalse:
            {
                Object* top_object = this->cur_context->pop_operand();

                bool exception_thrown = false; 
                Builtin::BoolObject* bool_result = dynamic_cast<Builtin::BoolObject*>(top_object);
                if (bool_result == nullptr)
                {
                    BaseFunction* bool_method;
                    TryGetMethodResult operation_result =  top_object->try_get_method(
                        MagicMethodNames::bool_method_name, bool_method);

                    switch (operation_result)
                    {
                    case TryGetMethodResult::NotFound:
                        bool_result = XyA_Allocate(Builtin::BoolObject, false);
                        break;

                    case TryGetMethodResult::NotCallable:
                        this->__throw_exception("__bool__ is not callable");
                        break;
                    
                    default:
                    {
                        break;
                    }
                    }
                }

                if (!bool_result->value)
                {
                    // 在执行完该函数后有this->cur_context->instruction_ptr ++语句
                    // 所以要减去一以在下一轮循环中跳转到正确位置
                    this->cur_context->instruction_ptr = instruction->parameter - 1;
                }

                bool_result->deallocate_if_no_ref();

                break;
            }

            case InstructionType::JumpTo:
            {
                // 在执行完该函数后有this->cur_context->instruction_ptr ++语句
                // 所以要减去一以在下一轮循环中跳转到正确位置
                this->cur_context->instruction_ptr = instruction->parameter - 1;
                break;
            }

            case InstructionType::PopTop:
            {
                Object* top_object = this->cur_context->pop_operand();
                top_object->deallocate_if_no_ref();
                break;
            }

            case InstructionType::Return:
            {
                this->cur_context->returned_obj = this->cur_context->pop_operand();
                this->cur_context->returned_obj->reference();
                break;
            }

            case InstructionType::CallFunction:
            {
                Object** args = new Object*[instruction->parameter];
                for (size_t i = 0; i < instruction->parameter; i ++)
                {
                    args[instruction->parameter - i - 1] = this->cur_context->pop_operand();
                }
                BaseFunction* callee = dynamic_cast<BaseFunction*>(this->cur_context->pop_operand());
                bool exception_thrown = false; 
                Object* result = callee->call(args, instruction->parameter, exception_thrown);

                if (exception_thrown)
                {
                    Builtin::BuiltinException* exception = static_cast<Builtin::BuiltinException*>(result);
                    this->__throw_exception(exception->message);
                }
                
                this->cur_context->push_operand(result);
                break;
            }

            case InstructionType::CallMethod:
            {
                Object** args = new Object*[instruction->parameter + 1];
                for (size_t i = 0; i < instruction->parameter; i ++)
                {
                    args[instruction->parameter - i] = this->cur_context->pop_operand();
                }
                BaseFunction* callee = dynamic_cast<BaseFunction*>(this->cur_context->pop_operand());
                args[0] = this->cur_context->pop_operand();
                bool exception_thrown = false; 
                Object* result = callee->call(args, instruction->parameter + 1, exception_thrown);

                if (exception_thrown)
                {
                    Builtin::BuiltinException* exception = static_cast<Builtin::BuiltinException*>(result);
                    this->__throw_exception(exception->message);
                }
                
                this->cur_context->push_operand(result);
                break;
            }
            
            default:
                break;
            }
        }

        void VirtualMachine::__back_context()
        {
            Context* back = this->cur_context->back;
            delete this->cur_context;
            this->cur_context = back;
        }

        void VirtualMachine::__call_binary_operation_magic_method(const std::string& magic_method_name)
        {
            Object* obj_2 = this->cur_context->pop_operand();
            Object* obj_1 = this->cur_context->pop_operand();

            BaseFunction* method;
            TryGetMethodResult result = obj_1->try_get_method(magic_method_name, method);

            switch (result)
            {
            case TryGetMethodResult::NotFound:
                this->__throw_exception("The method was not found");
                break;
            
            case TryGetMethodResult::NotCallable:
                this->__throw_exception("The method was not callable");
                break;
            }

            Object** args = XyA_Allocate_Array(Object*, 2, obj_1, obj_2);
            bool exception_thrown = false;
            Object* result_obj = method->call(args, 2, exception_thrown);
            XyA_Deallocate_Array(args);

            if (exception_thrown)
            {
                Builtin::BuiltinException* exception = static_cast<Builtin::BuiltinException*>(result_obj);
                this->__throw_exception(exception->message);
            }
            
            this->cur_context->push_operand(result_obj);
        }

        void VirtualMachine::__call_compare_magic_method(const std::string& magic_method_name)
        {
            Object* obj_2 = this->cur_context->pop_operand();
            Object* obj_1 = this->cur_context->pop_operand();

            BaseFunction* method;
            TryGetMethodResult result = obj_1->try_get_method(magic_method_name, method);

            switch (result)
            {
            case TryGetMethodResult::NotFound:
                this->__throw_exception("The method was not found");
                break;
            
            case TryGetMethodResult::NotCallable:
                this->__throw_exception("The method was not callable");
                break;
            }

            Object** args = XyA_Allocate_Array(Object*, 2, obj_1, obj_2);
            bool exception_thrown = false;
            Object* result_obj = method->call(args, 2, exception_thrown);
            XyA_Deallocate_Array(args);

            if (exception_thrown)
            {
                Builtin::BuiltinException* exception = static_cast<Builtin::BuiltinException*>(result_obj);
                this->__throw_exception(exception->message);
            }

            Builtin::BoolObject* bool_result = dynamic_cast<Builtin::BoolObject*>(result_obj);
            if (bool_result == nullptr)
            {
                this->__throw_exception("The type of the return value of the method is not bool");
            }
            this->cur_context->push_operand(bool_result);            
        }

        void VirtualMachine::__throw_exception(std::string_view message) const
        {
            for (auto callback : this->exception_callbacks)
            {
                callback(message);
            }
        }
    }
}