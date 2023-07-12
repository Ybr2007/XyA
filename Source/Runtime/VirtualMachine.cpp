#pragma once
#include <format>
#include <Runtime/MemoryManager.hpp>
#include <Runtime/VirtualMachine.h>
#include <Runtime/FunctionalUtils.hpp>


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
                // printf("Instruction %zd, %s\n", (size_t)VirtualMachine::get_instance()->cur_context, cur_instruction->to_string().c_str());
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
                const std::string& attr_name = this->cur_context->code_obj->attr_names[instruction->parameter];

                Object* attr_owner = this->cur_context->top_operand();

                Attr attr;
                TryGetAttrResult operation_result = attr_owner->try_get_attr(attr_name, attr);

                if (operation_result == TryGetAttrResult::NotFound)
                {
                    this->__throw_exception(std::format("The attr '{}' is not defined or already deleted.", attr_name));
                }

                if (attr.visibility == AttrVisibility::Private && this->cur_context->code_obj->cls != attr_owner->type())
                {
                    this->__throw_exception(std::format("Can not access private attr '{}'", attr_name));
                }

                this->cur_context->set_top_operand(attr.object);
                break;
            }

            case InstructionType::GetMethod:
            {
                const std::string& attr_name = this->cur_context->code_obj->attr_names[instruction->parameter];

                Object* attr_owner = this->cur_context->top_operand();

                Attr attr;
                TryGetAttrResult operation_result = attr_owner->try_get_attr(attr_name, attr);

                if (operation_result == TryGetAttrResult::NotFound)
                {
                    this->__throw_exception(std::format("The attr '{}' is not defined or already deleted.", attr_name));
                }

                if (attr.visibility == AttrVisibility::Private && this->cur_context->code_obj->cls != attr_owner->type())
                {
                    this->__throw_exception(std::format("Can not access the private attr '{}'", attr_name));
                }

                this->cur_context->push_operand(attr.object);
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
                const std::string& attr_name = this->cur_context->code_obj->attr_names[instruction->parameter];

                Object* new_attr = this->cur_context->pop_operand();
                Object* attr_owner = this->cur_context->pop_operand();
                
                Attr old_attr;
                TryGetAttrResult operation_result = attr_owner->try_get_attr(attr_name, old_attr);

                if (operation_result == TryGetAttrResult::OK)
                {
                    if (old_attr.visibility == AttrVisibility::Private && this->cur_context->code_obj->cls != attr_owner->type())
                    {
                        this->__throw_exception(std::format("Can not access the private attr '{}'", attr_name));
                    }
                }
                else
                {
                    if (!attr_owner->type()->instance_allow_external_attr)
                    {
                        this->__throw_exception(
                            std::format("Variables of type '{}' do not allow external attribute addition", attr_owner->type()->name));
                    }
                    else
                    {
                        old_attr.visibility = AttrVisibility::Public;
                    }
                }

                attr_owner->set_attr(attr_name, new_attr, old_attr.visibility);

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
                Builtin::BoolObject* bool_result;

                if (top_object->type() == Builtin::BoolType::get_instance())
                {
                    bool_result = static_cast<Builtin::BoolObject*>(top_object);
                }
                else
                {
                    BaseFunction* bool_method;
                    AttrVisibility visibility;
                    TryGetMethodResult operation_result =  top_object->try_get_method(
                        MagicMethodNames::bool_method_name, bool_method, visibility);

                    switch (operation_result)
                    {
                    case TryGetMethodResult::NotFound:
                        bool_result = XyA_Allocate(Builtin::BoolObject, false);
                        break;

                    case TryGetMethodResult::NotCallable:
                        this->__throw_exception("__bool__ is not callable");
                        break;
                    
                    case TryGetMethodResult::OK:
                    {
                        Object** args = XyA_Allocate_Array(Object*, 1, top_object);
                        Object* bool_method_return_object = bool_method->call(args, 1, exception_thrown);
                        XyA_Deallocate_Array(args, 1);

                        if (bool_method_return_object->type() != Builtin::BoolType::get_instance())
                        {
                            this->__throw_exception("The type of the return object of the __bool__ is not bool");
                        }
                        else
                        {
                            bool_result = static_cast<Builtin::BoolObject*>(bool_method_return_object);
                        }
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
                Object* callee_object = this->cur_context->pop_operand();
                Object** args;
                size_t arg_num;
                bool exception_thrown = false;
                Object* return_object;

                // 被调用的对象是类型，则调用__new__魔术方法初始化一个该类型的示例，并将类型对象本身作为第一个参数传入__new__方法
                if (callee_object->type() == Type::get_instance())
                {
                    arg_num = instruction->parameter + 1;
                    args = XyA_Allocate_Array_(Object*, arg_num);
                    args[0] = callee_object;
                    for (size_t i = 1; i <= instruction->parameter; i ++)
                    {
                        args[i] = this->cur_context->pop_operand();
                    }

                    BaseFunction* new_method;
                    AttrVisibility visibility;
                    auto result = callee_object->try_get_method(MagicMethodNames::new_method_name, new_method, visibility);

                    if (result != TryGetMethodResult::OK)
                    {
                        this->__throw_exception("Cannot call the __new__");
                    }

                    return_object = new_method->call(args, arg_num, exception_thrown);
                }
                else if (is_function(callee_object))
                {
                    BaseFunction* callee_function = static_cast<BaseFunction*>(callee_object);
                    arg_num = instruction->parameter;

                    args = XyA_Allocate_Array_(Object*, arg_num);
                    for (size_t i = 0; i < arg_num; i ++)
                    {
                        args[arg_num - i - 1] = this->cur_context->pop_operand();
                    }

                    return_object = callee_function->call(args, arg_num, exception_thrown);
                }
                XyA_Deallocate_Array(args, arg_num);

                if (exception_thrown)
                {
                    Builtin::BuiltinException* exception = static_cast<Builtin::BuiltinException*>(return_object);
                    this->__throw_exception(exception->message);
                }
                
                this->cur_context->push_operand(return_object);
                break;
            }

            case InstructionType::CallMethod:
            {
                Object* callee_object = this->cur_context->pop_operand();
                Object** args = XyA_Allocate_Array_(Object*, instruction->parameter + 1);
                args[0] = this->cur_context->pop_operand();
                for (size_t i = 0; i < instruction->parameter; i ++)
                {
                    args[instruction->parameter - i] = this->cur_context->pop_operand();
                }
                BaseFunction* callee = dynamic_cast<BaseFunction*>(callee_object);
                bool exception_thrown = false; 
                Object* result = callee->call(args, instruction->parameter + 1, exception_thrown);
                XyA_Deallocate_Array(args, instruction->parameter + 1);

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
            XyA_Deallocate(this->cur_context);
            this->cur_context = back;
        }

        void VirtualMachine::__call_binary_operation_magic_method(const std::string& magic_method_name)
        {
            Object* obj_2 = this->cur_context->pop_operand();
            Object* obj_1 = this->cur_context->pop_operand();

            BaseFunction* method;
            AttrVisibility visibility;
            TryGetMethodResult result = obj_1->try_get_method(magic_method_name, method, visibility);

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
            XyA_Deallocate_Array(args, 2);

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
            AttrVisibility visibility;
            TryGetMethodResult result = obj_1->try_get_method(magic_method_name, method, visibility);

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
            XyA_Deallocate_Array(args, 2);

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