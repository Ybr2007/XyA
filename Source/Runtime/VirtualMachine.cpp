#pragma once
#include <Runtime/MemoryManager.hpp>
#include <Runtime/VirtualMachine.h>


namespace XyA
{
    namespace Runtime
    {
        VirtualMachine::VirtualMachine()  // private
        {
        
        }

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

            case InstructionType::StroeVariable:
            {
                Object*& old_variable = this->cur_context->local_variables[instruction->parameter];
                if (old_variable != nullptr)
                {
                    old_variable->dereference();
                }

                this->cur_context->local_variables[instruction->parameter] = this->cur_context->pop_operand();
                this->cur_context->local_variables[instruction->parameter]->reference();

                break;
            }

            case InstructionType::BinaryAdd:
            {
                Object* obj_2 = this->cur_context->pop_operand();
                Object* obj_1 = this->cur_context->pop_operand();

                BaseFunction* add_method = this->__get_obj_method(obj_1, MagicMethodNames::add_method_name);

                Object** args = new Object*[2]{obj_1, obj_2};
                bool exception_thrown = false;
                Object* result_obj = add_method->call(args, 2, exception_thrown);
                delete[] args;

                if (exception_thrown)
                {
                    Builtin::BuiltinException* exception = dynamic_cast<Builtin::BuiltinException*>(result_obj);
                    this->__throw_exception(exception->message);
                }
                
                this->cur_context->push_operand(result_obj);
                break;
            }

            case InstructionType::BinarySubtract:
            {
                Object* obj_2 = this->cur_context->pop_operand();
                Object* obj_1 = this->cur_context->pop_operand();

                BaseFunction* sub_method = this->__get_obj_method(obj_1, MagicMethodNames::subtract_method_name);

                Object** args = new Object*[2]{obj_1, obj_2};
                bool exception_thrown = false;
                Object* result_obj = sub_method->call(args, 2, exception_thrown);
                delete[] args;

                if (exception_thrown)
                {
                    Builtin::BuiltinException* exception = dynamic_cast<Builtin::BuiltinException*>(result_obj);
                    this->__throw_exception(exception->message);
                }
                
                this->cur_context->push_operand(result_obj);
                break;
            }

            case InstructionType::BinaryMultiply:
            {
                Object* obj_2 = this->cur_context->pop_operand();
                Object* obj_1 = this->cur_context->pop_operand();

                BaseFunction* mul_method = this->__get_obj_method(obj_1, MagicMethodNames::multiply_method_name);

                Object** args = new Object*[2]{obj_1, obj_2};
                bool exception_thrown = false;
                Object* result_obj = mul_method->call(args, 2, exception_thrown);
                delete[] args;

                if (exception_thrown)
                {
                    Builtin::BuiltinException* exception = dynamic_cast<Builtin::BuiltinException*>(result_obj);
                    this->__throw_exception(exception->message);
                }
                
                this->cur_context->push_operand(result_obj);
                break;
            }

            case InstructionType::BinaryDevide:
            {
                Object* obj_2 = this->cur_context->pop_operand();
                Object* obj_1 = this->cur_context->pop_operand();

                BaseFunction* div_method = this->__get_obj_method(obj_1, MagicMethodNames::divide_method_name);

                Object** args = new Object*[2]{obj_1, obj_2};
                bool exception_thrown = false;
                Object* result_obj = div_method->call(args, 2, exception_thrown);
                delete[] args;

                if (exception_thrown)
                {
                    Builtin::BuiltinException* exception = dynamic_cast<Builtin::BuiltinException*>(result_obj);
                    this->__throw_exception(exception->message);
                }
                
                this->cur_context->push_operand(result_obj);
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
                    BaseFunction* bool_method = this->__get_obj_method(top_object, MagicMethodNames::bool_method_name);
                    
                    Object** args = new Object*[1]{top_object};
                    Object* result = bool_method->call(args, 1, exception_thrown);
                    delete[] args;

                    if (exception_thrown)
                    {
                        Builtin::BuiltinException* exception = dynamic_cast<Builtin::BuiltinException*>(result);
                        this->__throw_exception(exception->message);
                    }

                    bool_result = dynamic_cast<Builtin::BoolObject*>(result);

                    if (bool_result == nullptr)
                    {
                        this->__throw_exception("The type of return value of the __bool__ method was not bool");
                    }
                }

                if (!bool_result->value)
                {
                    // 在执行完该函数后有this->cur_context->instruction_ptr ++语句
                    // 所以要减去一以在下一轮循环中跳转到正确位置
                    this->cur_context->instruction_ptr = instruction->parameter - 1;
                }

                if (bool_result->ref_count == 0)
                {
                    XyA_Deallocate(bool_result);
                }

                break;
            }

            case InstructionType::JumpForward:
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
                    Builtin::BuiltinException* exception = dynamic_cast<Builtin::BuiltinException*>(result);
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

        void VirtualMachine::__call_compare_magic_method(const std::string& method_name)
        {
            Object* obj_2 = this->cur_context->pop_operand();
            Object* obj_1 = this->cur_context->pop_operand();

            BaseFunction* equal_method = this->__get_obj_method(obj_1, method_name);

            Object** args = new Object*[2]{obj_1, obj_2};
            bool exception_thrown = false;
            Object* result_obj = equal_method->call(args, 2, exception_thrown);
            delete[] args;

            if (exception_thrown)
            {
                Builtin::BuiltinException* exception = dynamic_cast<Builtin::BuiltinException*>(result_obj);
                this->__throw_exception(exception->message);
            }

            if (result_obj == nullptr)
            {
                this->__throw_exception("The type of the return value of the " + method_name + " method is not bool");
            }
            this->cur_context->push_operand(result_obj);            
        }


        BaseFunction* VirtualMachine::__get_obj_method(Object* object, const std::string& method_name) const
        {
            BaseFunction* method;
            auto result = object->try_get_method(method_name, method);
            switch (result)
            {
            case TryGetMethodResult::NotFound:
                this->__throw_exception("The method " + method_name + " was not found");
                break;
            
            case TryGetMethodResult::NotCallable:
                this->__throw_exception("The method " + method_name + " was not callable");
                break;

            default:
                return method;
            }
            return nullptr;
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