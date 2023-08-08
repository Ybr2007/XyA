#include <format>
#include <Runtime/MemoryManager.hpp>
#include <Runtime/VirtualMachine.h>
#include <Runtime/FunctionalUtils.hpp>
#include <Runtime/NameMapping.h>
#include <Runtime/Module.h>


namespace XyA
{
    namespace Runtime
    {
        VirtualMachine* VirtualMachine::get_instance()
        {
            static VirtualMachine instance;
            return &instance;
        }

        void VirtualMachine::execute(CodeObject* global_code_object)
        {
            this->global_context = XyA_Allocate(Context, global_code_object);
            this->cur_context = this->global_context;

            this->__init_global_context();
            this->execute_context();

            XyA_Deallocate(global_code_object);
        }

        void VirtualMachine::execute_context()
        {
            while (this->cur_context->instruction_ptr < this->cur_context->code_obj->instructions.size())
            {
                Instruction* cur_instruction = this->cur_context->cur_instruction();
                this->__excute_instruction(cur_instruction);

                if (this->cur_context == nullptr)
                {
                    return;
                }

                this->cur_context->instruction_ptr ++;
            }
            this->__back_context();
        }

        void VirtualMachine::__init_global_context()
        {
            this->global_context->local_variables[0] = Builtin::IntType::get_instance();  // 0: global_context->code_obj->variable_name_indices["int"]
            this->global_context->local_variables[1] = Builtin::FloatType::get_instance();  // 1: global_context->code_obj->variable_name_indices["float"]
            this->global_context->local_variables[2] = Builtin::StringType::get_instance();  // 2: global_context->code_obj->variable_name_indices["str"]
            this->global_context->local_variables[3] = Builtin::BoolType::get_instance();  // 3: global_context->code_obj->variable_name_indices["bool"]

            Object*& builtin_print_function = this->global_context->local_variables[4];  // 4: global_context->code_obj->variable_name_indices["print"]
            builtin_print_function = XyA_Allocate(Builtin::BuiltinFunction, Builtin::print);
            builtin_print_function->reference();

            Object*& builtin__get_ref_count_function = this->global_context->local_variables[5];  // 5: global_context->code_obj->variable_name_indices["_get_ref_count"]
            builtin__get_ref_count_function = XyA_Allocate(Builtin::BuiltinFunction, Builtin::_get_ref_count);
            builtin__get_ref_count_function->reference();

            Object*& builtin__get_id_function = this->global_context->local_variables[6];  // 6: global_context->code_obj->variable_name_indices["_get_id"]
            builtin__get_id_function = XyA_Allocate(Builtin::BuiltinFunction, Builtin::_get_id);
            builtin__get_id_function->reference();

            Object*& builtin_clock_function = this->global_context->local_variables[7];  // 7: global_context->code_obj->variable_name_indices["clock"]
            builtin_clock_function = XyA_Allocate(Builtin::BuiltinFunction, Builtin::clock_);
            builtin_clock_function->reference();

            Object*& builtin_sizeof_function = this->global_context->local_variables[8];  // 8: global_context->code_obj->variable_name_indices["clock"]
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
                    std::string_view variable_name = this->cur_context->get_variable_name_at(instruction->parameter);
                    this->cur_context->set_exception(
                        XyA_Allocate(Builtin::BuiltinException, std::format("The variable '{}' is not defined or already deleted.", variable_name))
                    );
                    goto error;
                }
                this->cur_context->push_operand(variable);
                break;
            }

            case InstructionType::LoadGlobal:
            {
                Object* variable = this->global_context->get_variable_at(instruction->parameter);
                if (variable == nullptr)
                {
                    std::string_view variable_name = this->cur_context->get_variable_name_at(instruction->parameter);
                    this->cur_context->set_exception(
                        XyA_Allocate(Builtin::BuiltinException, std::format("The variable '{}' is not defined or already deleted.", variable_name))
                    );
                    goto error;
                }
                this->cur_context->push_operand(variable);
                break;
            }

            case InstructionType::GetAttr:
            {
                Object* attr_owner = this->cur_context->top_operand();

                Attr attr;
                TryGetAttrResult operation_result = attr_owner->try_get_attr(instruction->parameter, attr);

                if (operation_result == TryGetAttrResult::NotFound)
                {
                    StringView attr_name = NameMapper::get_instance().get_name(instruction->parameter);
                    this->cur_context->set_exception(
                        XyA_Allocate(Builtin::BuiltinException, std::format("The attr '{}' is not defined or already deleted.", attr_name))
                    );
                    goto error;
                }

                if (attr.accessibility == AttrAccessibility::Private && this->cur_context->cls() != attr_owner->type())
                {
                    StringView attr_name = NameMapper::get_instance().get_name(instruction->parameter);
                    this->cur_context->set_exception(
                        XyA_Allocate(Builtin::BuiltinException, std::format("Can not access the private attr '{}'", attr_name))
                    );
                    goto error;
                }

                this->cur_context->set_top_operand(attr.object);
                break;
            }

            case InstructionType::GetMethod:
            {
                Object* attr_owner = this->cur_context->top_operand();

                Attr attr;
                TryGetAttrResult operation_result = attr_owner->try_get_attr(instruction->parameter, attr);

                if (operation_result == TryGetAttrResult::NotFound)
                {
                    StringView attr_name = NameMapper::get_instance().get_name(instruction->parameter);
                    this->cur_context->set_exception(
                        XyA_Allocate(Builtin::BuiltinException, std::format("The attr '{}' is not defined or already deleted.", attr_name))
                    );
                    goto error;
                }

                if (attr.accessibility == AttrAccessibility::Private && this->cur_context->cls() != attr_owner->type())
                {
                    StringView attr_name = NameMapper::get_instance().get_name(instruction->parameter);
                    this->cur_context->set_exception(
                        XyA_Allocate(Builtin::BuiltinException, std::format("Can not access the private attr '{}'", attr_name))
                    );
                    goto error;
                }

                this->cur_context->push_operand(attr.object);
                break;
            }

            case InstructionType::StroeVariable:
            {
                this->cur_context->set_variable_at(instruction->parameter, this->cur_context->pop_operand());
                break;
            }

            case InstructionType::StorePublicAttr:
            case InstructionType::StroePrivateAttr:
            {
                Object* new_attr = this->cur_context->pop_operand();
                Object* attr_owner = this->cur_context->pop_operand();

                Attr old_attr;
                TryGetAttrResult operation_result = attr_owner->try_get_attr(instruction->parameter, old_attr);

                if (operation_result == TryGetAttrResult::OK)
                {
                    if (old_attr.accessibility == AttrAccessibility::Private && this->cur_context->code_obj->cls != attr_owner->type())
                    {
                    StringView attr_name = NameMapper::get_instance().get_name(instruction->parameter);
                        this->cur_context->set_exception(
                            XyA_Allocate(Builtin::BuiltinException, std::format("Can not access the private attr '{}'", attr_name))
                        );
                        goto error;
                    }
                }
                else
                {
                    if (!attr_owner->type()->allow_ext_attr_add && this->cur_context->code_obj->cls != attr_owner->type())
                    {
                        this->cur_context->set_exception(
                            XyA_Allocate(Builtin::BuiltinException,
                                std::format("Objects of type '{}' do not allow external attribute addition", attr_owner->type()->name))
                        );
                        goto error;
                    }
                    else
                    {
                        old_attr.accessibility = instruction->type == InstructionType::StorePublicAttr ? AttrAccessibility::Public : AttrAccessibility::Private;
                    }
                }

                attr_owner->set_attr(instruction->parameter, new_attr, old_attr.accessibility);

                break;
            }

            case InstructionType::BinaryAdd:
            {
                this->__call_binary_operation_magic_method(MagicMethodNames::add_method_name_id);
                break;
            }

            case InstructionType::BinarySubtract:
            {
                this->__call_binary_operation_magic_method(MagicMethodNames::subtract_method_name_id);
                break;
            }

            case InstructionType::BinaryMultiply:
            {
                this->__call_binary_operation_magic_method(MagicMethodNames::multiply_method_name_id);
                break;
            }

            case InstructionType::BinaryDevide:
            {
                this->__call_binary_operation_magic_method(MagicMethodNames::divide_method_name_id);
                break;
            }

            case InstructionType::CompareIfEqual:
            {
                this->__call_compare_magic_method(MagicMethodNames::equal_method_name_id);
                break;
            }

            case InstructionType::CompareIfGreaterThan:
            {
                this->__call_compare_magic_method(MagicMethodNames::greater_method_name_id);
                break;
            }

            case InstructionType::CompareIfGreaterEqual:
            {
                this->__call_compare_magic_method(MagicMethodNames::greater_equal_method_name_id);
                break;
            }

            case InstructionType::CompareIfLessThan:
            {
                this->__call_compare_magic_method(MagicMethodNames::less_method_name_id);
                break;
            }

            case InstructionType::CompareIfLessEqual:
            {
                this->__call_compare_magic_method(MagicMethodNames::less_equal_method_name_id);
                break;
            }

            case InstructionType::CompareIfNotEqual:
            {
                this->__call_compare_magic_method(MagicMethodNames::not_equal_method_name_id);
                break;
            }

            case InstructionType::PopJumpIfFalse:
            {
                Object* top_object = this->cur_context->pop_operand();

                bool exception_thrown = false;
                Builtin::BoolObject* bool_value;

                if (top_object->is_instance(Builtin::BoolType::get_instance()))
                {
                    bool_value = static_cast<Builtin::BoolObject*>(top_object);
                }
                else
                {
                    BaseFunction* bool_method;
                    AttrAccessibility accessibility;
                    TryGetMethodResult operation_result =  top_object->try_get_method(MagicMethodNames::bool_method_name_id, bool_method, accessibility);

                    if (accessibility == AttrAccessibility::Private && this->cur_context->cls() != top_object->type())
                    {
                        this->cur_context->set_exception(
                            XyA_Allocate(Builtin::BuiltinException, std::format("Can not access the private attr '__bool__'"))
                        );
                        goto error;
                    }

                    switch (operation_result)
                    {
                    case TryGetMethodResult::NotFound:
                        bool_value = Builtin::BoolObject::get_instance(false);
                        break;

                    case TryGetMethodResult::NotCallable:
                        this->cur_context->set_exception(
                            XyA_Allocate(Builtin::BuiltinException, "The method '__bool__' is not callable")
                        );
                        goto error;

                    case TryGetMethodResult::OK:
                    {
                        Object** args = XyA_Allocate_Array(Object*, 1, top_object);
                        Object* bool_method_return_value = bool_method->call(args, 1, exception_thrown);
                        XyA_Deallocate_Array(args, 1);

                        if (bool_method_return_value->type() != Builtin::BoolType::get_instance())
                        {
                            this->cur_context->set_exception(
                                XyA_Allocate(Builtin::BuiltinException, "The return value of the method '__bool__' is not of bool type")
                            );
                            goto error;
                        }
                        else
                        {
                            bool_value = static_cast<Builtin::BoolObject*>(bool_method_return_value);
                        }
                    }
                    }
                }

                if (!bool_value->value)
                {
                    // 在执行完该函数后有this->cur_context->instruction_ptr ++语句
                    // 所以要减去一以在下一轮循环中跳转到正确位置
                    this->cur_context->instruction_ptr = instruction->parameter - 1;
                }

                bool_value->deallocate_if_no_ref();

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

            case InstructionType::ConvertType:
            {
                Object* target_type_object = this->cur_context->pop_operand();
                if (!target_type_object->is_instance(Type::get_instance()))
                {
                    this->cur_context->set_exception("The type-conversion target is not type");
                    this->__throw_exception();
                    return;
                }

                bool convert_to_string = target_type_object == Builtin::StringType::get_instance();
                bool convert_to_bool = target_type_object == Builtin::BoolType::get_instance();

                Object* original_object = this->cur_context->top_operand();

                size_t method_name_id;
                if (convert_to_string)
                {
                    method_name_id = MagicMethodNames::str_method_name_id;
                }
                else if (convert_to_bool)
                {
                    method_name_id = MagicMethodNames::bool_method_name_id;
                }
                else
                {
                    method_name_id = MagicMethodNames::as_method_name_id;
                }

                BaseFunction* method;
                AttrAccessibility accessibility;
                TryGetMethodResult result = original_object->try_get_method(method_name_id, method, accessibility);

                switch (result)
                {
                case TryGetMethodResult::NotFound:
                {
                    StringView method_name = NameMapper::get_instance().get_name(method_name_id);
                    this->cur_context->set_exception(std::format("The method '{}' was not found", method_name));
                    this->__throw_exception();
                    return;
                }

                case TryGetMethodResult::NotCallable:
                {
                    StringView method_name = NameMapper::get_instance().get_name(method_name_id);
                    this->cur_context->set_exception(std::format("The method '{}' was not callable", method_name));
                    this->__throw_exception();
                    return;
                }

                default:
                    break;
                }

                if (accessibility == AttrAccessibility::Private && this->cur_context->cls() != original_object->type())
                {
                    StringView method_name = NameMapper::get_instance().get_name(method_name_id);
                    this->cur_context->set_exception(std::format("Can not access the private attr '{}'", method_name));
                    this->__throw_exception();
                    return;
                }

                Object** args = (convert_to_string || convert_to_bool) ?
                    XyA_Allocate_Array(Object*, 1, original_object) : XyA_Allocate_Array(Object*, 2, original_object, target_type_object);
                size_t arg_num = (convert_to_string || convert_to_bool) ? 1 : 2;
                bool exception_thrown = false;

                Object* return_value = method->call(args, arg_num, exception_thrown);
                XyA_Deallocate_Array(args, arg_num);

                if (exception_thrown)
                {
                    this->cur_context->set_exception(static_cast<Builtin::BuiltinException*>(return_value));
                    this->__throw_exception();
                    return;
                }
                if (!return_value->is_instance(static_cast<Type*>(target_type_object)))
                {
                    this->cur_context->set_exception(
                        std::format("The return value of method '{}' is not of target type", MagicMethodNames::as_method_name));
                    this->__throw_exception();
                    return;
                }

                this->cur_context->set_top_operand(return_value);
                break;
            }

            case InstructionType::CallFunction:
            {
                Object* callee_object = this->cur_context->pop_operand();
                Object** args;
                size_t arg_num;
                bool exception_thrown = false;
                Object* return_value;

                // 被调用的对象是类型, 则调用__new__魔术方法初始化一个该类型的示例, 并将类型对象本身作为第一个参数传入__new__方法
                if (callee_object->is_instance(Type::get_instance()))
                {
                    arg_num = instruction->parameter + 1;
                    args = XyA_Allocate_Array_(Object*, arg_num);
                    args[0] = callee_object;
                    for (size_t i = 1; i <= instruction->parameter; i ++)
                    {
                        args[i] = this->cur_context->pop_operand();
                    }

                    BaseFunction* new_method;
                    AttrAccessibility accessibility;
                    auto result = callee_object->try_get_method(MagicMethodNames::new_method_name_id, new_method, accessibility);

                    if (result != TryGetMethodResult::OK)
                    {
                        this->cur_context->set_exception("Cannot call the method '__new__'");
                        goto error;
                    }

                    return_value = new_method->call(args, arg_num, exception_thrown);
                }
                else if (is_callable(callee_object))
                {
                    BaseFunction* callee_function = static_cast<BaseFunction*>(callee_object);
                    arg_num = instruction->parameter;

                    args = XyA_Allocate_Array_(Object*, arg_num);
                    for (size_t i = 0; i < arg_num; i ++)
                    {
                        args[arg_num - i - 1] = this->cur_context->pop_operand();
                    }

                    return_value = callee_function->call(args, arg_num, exception_thrown);
                }
                else
                {
                    this->cur_context->set_exception("The callee object is not callable");
                    goto error;
                }
                XyA_Deallocate_Array(args, arg_num);

                if (exception_thrown)
                {
                    this->cur_context->set_exception(static_cast<Builtin::BuiltinException*>(return_value));
                    goto error;
                }

                this->cur_context->push_operand(return_value);
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
                BaseFunction* callee = static_cast<BaseFunction*>(callee_object);  // TODO
                bool exception_thrown = false;
                Object* return_value = callee->call(args, instruction->parameter + 1, exception_thrown);
                XyA_Deallocate_Array(args, instruction->parameter + 1);

                if (exception_thrown)
                {
                    this->cur_context->set_exception(static_cast<Builtin::BuiltinException*>(return_value));
                }

                this->cur_context->push_operand(return_value);
                break;
            }

            case InstructionType::ImportModule:
            {
                auto [module_code_object, module_variable_index] = 
                    this->cur_context->code_obj->module_datas[instruction->parameter];
                Module* module_object = XyA_Allocate(Module, module_code_object);

                this->cur_context->set_variable_at(module_variable_index, module_object);
            }

            default:
                break;
            }

            return;

            error:
            this->__throw_exception();
        }

        void VirtualMachine::__back_context()
        {
            Context* back = this->cur_context->back;
            XyA_Deallocate(this->cur_context);
            this->cur_context = back;
        }

        void VirtualMachine::__call_binary_operation_magic_method(size_t magic_method_name_id)
        {
            Object* obj_2 = this->cur_context->pop_operand();
            Object* obj_1 = this->cur_context->top_operand();

            BaseFunction* method;
            AttrAccessibility accessibility;
            TryGetMethodResult result = obj_1->try_get_method(magic_method_name_id, method, accessibility);

            switch (result)
            {
            case TryGetMethodResult::NotFound:
                this->cur_context->set_exception(
                    XyA_Allocate(Builtin::BuiltinException, "The method was not found")
                );
                this->__throw_exception();
                return;

            case TryGetMethodResult::NotCallable:
                this->cur_context->set_exception(
                    XyA_Allocate(Builtin::BuiltinException, "The method was not callable")
                );
                this->__throw_exception();
                return;

            default:
                break;
            }

            if (accessibility == AttrAccessibility::Private && this->cur_context->cls() != obj_1->type())
            {
                StringView magic_method_name = NameMapper::get_instance().get_name(magic_method_name_id);
                this->cur_context->set_exception(
                    XyA_Allocate(Builtin::BuiltinException, std::format("Can not access the private attr '{}'", magic_method_name))
                );
                this->__throw_exception();
                return;
            }

            Object** args = XyA_Allocate_Array(Object*, 2, obj_1, obj_2);
            bool exception_thrown = false;
            Object* return_value = method->call(args, 2, exception_thrown);
            XyA_Deallocate_Array(args, 2);

            if (exception_thrown)
            {
                this->cur_context->set_exception(static_cast<Builtin::BuiltinException*>(return_value));
                this->__throw_exception();
                return;
            }

            this->cur_context->set_top_operand(return_value);
        }

        void VirtualMachine::__call_compare_magic_method(size_t magic_method_name_id)
        {
            Object* obj_2 = this->cur_context->pop_operand();
            Object* obj_1 = this->cur_context->top_operand();

            BaseFunction* method;
            AttrAccessibility accessibility;
            TryGetMethodResult result = obj_1->try_get_method(magic_method_name_id, method, accessibility);

            if (accessibility == AttrAccessibility::Private && this->cur_context->cls() != obj_1->type())
            {
                StringView magic_method_name = NameMapper::get_instance().get_name(magic_method_name_id);
                this->cur_context->set_exception(
                    XyA_Allocate(Builtin::BuiltinException, std::format("Can not access the private attr '{}'", magic_method_name))
                );
                this->__throw_exception();
                return;
            }

            switch (result)
            {
            case TryGetMethodResult::NotFound:
                this->cur_context->set_exception(
                    XyA_Allocate(Builtin::BuiltinException, "The method was not found")
                );
                this->__throw_exception();
                return;

            case TryGetMethodResult::NotCallable:
                this->cur_context->set_exception(
                    XyA_Allocate(Builtin::BuiltinException, "The method was not callable")
                );
                this->__throw_exception();
                return;

            default:
                break;
            }

            Object** args = XyA_Allocate_Array(Object*, 2, obj_1, obj_2);
            bool exception_thrown = false;
            Object* return_value = method->call(args, 2, exception_thrown);
            XyA_Deallocate_Array(args, 2);

            if (exception_thrown)
            {
                this->cur_context->set_exception(static_cast<Builtin::BuiltinException*>(return_value));
                this->__throw_exception();
                return;
            }
            if (!return_value->is_instance(Builtin::BoolType::get_instance()))
            {
                StringView magic_method_name = NameMapper::get_instance().get_name(magic_method_name_id);
                this->cur_context->set_exception(
                    XyA_Allocate(Builtin::BuiltinException, std::format("The return value of the method '{}' is not of bool type", magic_method_name))
                );
                this->__throw_exception();
                return;
            }

            Builtin::BoolObject* bool_value = static_cast<Builtin::BoolObject*>(return_value);

            this->cur_context->set_top_operand(bool_value);
        }

        void VirtualMachine::__throw_exception()
        {
            for (auto callback : this->exception_callbacks)
            {
                callback(this->cur_context->thrown_exception->message());
            }

            this->__back_context();
            exit(-1);
        }
    }
}