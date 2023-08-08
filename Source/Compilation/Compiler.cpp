#include <Compilation/Compiler.h>

#include <fstream>
#include <Exception/CoreException.h>
#include <Runtime/Type.h>
#include <Runtime/CustomFunction.h>
#include <Runtime/Builtin/Null.h>
#include <Runtime/VirtualMachine.h>
#include <Runtime/NameMapping.h>
#include <Utils/TemporarySetter.hpp>
#include <LexicalAnalysis/TokenAnalyzer.h>
#include <SyntaxAnalysis/SyntaxParser.h>


namespace XyA
{
    namespace Compilation
    {
        using namespace LexicalAnalysis;
        using namespace SyntaxAnalysis;
        using namespace Runtime;
        using CompilerErrorCallback = std::function<void(StringView, LexicalAnalysis::TokenPos)>;

        CodeObject* Compiler::compile(SyntaxTreeNode* syntax_tree_node)
        {
            // delete于Context::~Context
            this->__global_code_object = XyA_Allocate_(CodeObject);

            this->__init_builtins();

            for (auto unit : syntax_tree_node->children)
            {
                this->__compile_unit(this->__global_code_object, unit);
            }

            return this->__global_code_object;
        }

        void Compiler::register_message_callback(MessageCallback callback)
        {
            this->__message_callbacks.push_back(callback);
        }

        void Compiler::__init_builtins()
        {
            // 为global_code_object预留builtin objects的位置
            // 实际的builtin objects将会在VirtualMachine启动时被加入到为global_context的local_variables
            this->__global_code_object->register_variable_name("int");
            this->__global_code_object->register_variable_name("float");
            this->__global_code_object->register_variable_name("str");
            this->__global_code_object->register_variable_name("bool");
            this->__global_code_object->register_variable_name("print");
            this->__global_code_object->register_variable_name("_get_ref_count");
            this->__global_code_object->register_variable_name("_get_id");
            this->__global_code_object->register_variable_name("clock");
            this->__global_code_object->register_variable_name("sizeof");
        }

        void Compiler::__compile_unit(CodeObject* code_object, SyntaxTreeNode* unit_node)
        {
            switch (unit_node->type)
            {
            case SyntaxTreeNodeType::Block:
                this->__compile_block(code_object, unit_node);
                break;

            case SyntaxTreeNodeType::If:
                this->__compile_if(code_object, unit_node);
                break;

            case SyntaxTreeNodeType::While:
                this->__compile_while(code_object, unit_node);
                break;

            case SyntaxTreeNodeType::Function_Definition:
            {
                size_t function_variable_index;
                CustomFunction* function = this->__build_function(code_object, unit_node, function_variable_index);
                code_object->prebuilt_objects.emplace_back(function_variable_index, function);
                break;
            }

            case SyntaxTreeNodeType::Class_Definition:
            {
                size_t class_variable_index;
                Type* class_type = this->__build_class(code_object, unit_node, class_variable_index);
                code_object->prebuilt_objects.emplace_back(class_variable_index, class_type);
                break;
            }

            case SyntaxTreeNodeType::Return:
            {
                this->__compile_return(code_object, unit_node);
                break;
            }

            default:
                this->__compile_line(code_object, unit_node);
            }
        }

        void Compiler::__compile_block(CodeObject* code_object, SyntaxTreeNode* block_node)
        {
            for (auto unit : block_node->children)
            {
                this->__compile_unit(code_object, unit);
            }
        }

        void Compiler::__compile_if(CodeObject* code_object, SyntaxTreeNode* if_node)
        {
            this->__compile_expression(code_object, if_node->children[0]);

            Instruction* jump_if_false_instruction = new Instruction(InstructionType::PopJumpIfFalse);
            code_object->instructions.push_back(jump_if_false_instruction);

            auto _ = TemporarySetter(this->__inside_judgment_block, true);
                
            this->__compile_block(code_object, if_node->children[1]);
            jump_if_false_instruction->parameter = code_object->instructions.size();

            if (if_node->children.size() == 3)  // 有else语块
            {
                jump_if_false_instruction->parameter ++;

                Instruction* jump_forward_instruction = new Instruction(InstructionType::JumpTo);
                code_object->instructions.push_back(jump_forward_instruction);

                if (if_node->children[2]->type == SyntaxTreeNodeType::If)
                {
                    this->__compile_if(code_object, if_node->children[2]);
                }
                else  // if_node->children[2]->type == SyntaxTreeNodeType::Block
                {
                    this->__compile_block(code_object, if_node->children[2]);
                }

                jump_forward_instruction->parameter = code_object->instructions.size();
            }
        }

        void Compiler::__compile_while(CodeObject* code_object, SyntaxTreeNode* while_node)
        {
            size_t start_pos = code_object->instructions.size();
            this->__compile_expression(code_object, while_node->children[0]);

            Instruction* jump_if_false_instruction = new Instruction(InstructionType::PopJumpIfFalse);
            code_object->instructions.push_back(jump_if_false_instruction);

            auto _ = TemporarySetter(this->__inside_loop_block, true);

            this->__compile_block(code_object, while_node->children[1]);
            jump_if_false_instruction->parameter = code_object->instructions.size() + 1;  // +1是因为由Jump backward

            Instruction* jump_backward_instruction = new Instruction(InstructionType::JumpTo);
            jump_backward_instruction->parameter = start_pos;
            code_object->instructions.push_back(jump_backward_instruction);
        }

        void Compiler::__compile_line(CodeObject* code_object, SyntaxTreeNode* line_node)
        {
            if (line_node->type == SyntaxTreeNodeType::Import)
            {
                StringSource module_path = "";
                SyntaxTreeNode* imported_module_node = line_node->children[0];
                for (auto package_node : imported_module_node->children)
                {
                    module_path += package_node->token->value + "/";
                }
                module_path += imported_module_node->token->value + ".xya";

                StringView module_variable_name = imported_module_node->children.empty() ? 
                    imported_module_node->token->value : imported_module_node->children[0]->token->value;

                std::ifstream input_file_stream(module_path, std::ios::in);
                if (!input_file_stream.is_open())
                {
                    return this->__throw_error(
                        std::format("Can not open file '{}'", module_path),
                        imported_module_node->token->start_pos
                    );
                }

                std::stringstream buffer;  
                buffer << input_file_stream.rdbuf();  
                StringSource module_source = buffer.str();

                auto tokens = TokenAnalyzer().analyze_source(module_source);
                auto syntax_tree = SyntaxParser().parse_tokens(tokens);
                auto module_code_object = Compiler().compile(syntax_tree);

                Instruction* import_module_instruction = new Instruction(InstructionType::ImportModule);
                import_module_instruction->parameter = code_object->module_datas.size();
                code_object->module_datas.emplace_back(
                    module_code_object, code_object->get_variable_index(module_variable_name));
                code_object->instructions.push_back(import_module_instruction);
            }
            else if (line_node->type == SyntaxTreeNodeType::Assignment)
            {
                this->__compile_assignment(code_object, line_node);
            }
            else if (line_node->is_expression())
            {
                this->__compile_expression(code_object, line_node, true);
            }
        }

        void Compiler::__compile_assignment(CodeObject* code_object, SyntaxTreeNode* assignment_node)
        {
            if (assignment_node->children[0]->type == SyntaxTreeNodeType::Attr)
            {
                SyntaxTreeNode* attr_onwer_node = assignment_node->children[0]->children[0];

                this->__compile_expression(code_object, attr_onwer_node);
                this->__compile_expression(code_object, assignment_node->children[1]);

                bool has_accessibility_modifier = assignment_node->children[0]->children.size() == 2;

                if (has_accessibility_modifier)
                {
                    if (!this->__inside_constructor)
                    {
                        this->__throw_error(
                            "Can not use accessibility modifiers outside constructors", 
                            assignment_node->children[0]->children[1]->token->start_pos);
                        return;
                    }
                    if (this->__inside_judgment_block)
                    {
                        this->__throw_error(
                            "Can not use accessibility modifiers inside judgment blocks", 
                            assignment_node->children[0]->children[1]->token->start_pos);
                        return;
                    }
                    if (this->__inside_loop_block)
                    {
                        this->__throw_error(
                            "Can not use accessibility modifiers inside loop blocks", 
                            assignment_node->children[0]->children[1]->token->start_pos);
                        return;
                    }
                    if (attr_onwer_node->token->value != this->__first_argument_name_of_method)
                    {
                        this->__throw_error(
                            "Cannot specify the accessibility of attributes for objects of other classes", 
                            assignment_node->children[0]->children[1]->token->start_pos);
                        return;
                    }
                }

                Instruction* store_attr_instruction = new Instruction(
                    !has_accessibility_modifier || assignment_node->children[0]->children[1]->token->type == LexicalAnalysis::TokenType::Kw_Public ?
                    InstructionType::StorePublicAttr : InstructionType::StroePrivateAttr
                );


                StringView attr_name = assignment_node->children[0]->token->value;
                Id attr_name_id = NameMapper::get_instance().get_name_id(attr_name);
                store_attr_instruction->parameter = attr_name_id;

                // 不是第一个可能的带有可访问性修饰符的赋值语句
                if (has_accessibility_modifier && 
                    this->__assignments_inside_constructor.find(attr_name_id) != this->__assignments_inside_constructor.end())
                {
                    delete store_attr_instruction;

                    this->__throw_error(
                        "Assignments with modifiers are not the first possible assignment", 
                        assignment_node->children[0]->children[1]->token->start_pos);
                    return;
                }
                this->__assignments_inside_constructor.insert(attr_name_id);
                
                code_object->instructions.push_back(store_attr_instruction);
            }
            else  // assignment target is an Identifier
            {
                this->__compile_expression(code_object, assignment_node->children[1]);

                Instruction* store_variable_instruction = new Instruction(InstructionType::StroeVariable);
                store_variable_instruction->parameter = code_object->get_variable_index(assignment_node->children[0]->token->value);
                code_object->instructions.push_back(store_variable_instruction);
            }
        }

        void Compiler::__compile_expression(CodeObject* code_object, SyntaxTreeNode* expression_node, bool pop)
        {
            expression_node->try_fold_literal();

            if (expression_node->type == SyntaxTreeNodeType::Type_Conversion)
            {
                this->__compile_expression(code_object, expression_node->children[0]);  // 计算被转换对象的值
                this->__compile_expression(code_object, expression_node->children[1]);  // 计算目标类型对象的值
                Instruction* convert_type_instruction = new Instruction(InstructionType::ConvertType);
                code_object->instructions.push_back(convert_type_instruction);

                if (pop)
                {
                    Instruction* pop_top_instruction = new Instruction(InstructionType::PopTop);
                    code_object->instructions.push_back(pop_top_instruction);
                }
                return;
            }

            if (expression_node->type == SyntaxTreeNodeType::Attr)
            {
                this->__compile_expression(code_object, expression_node->children[0]);  // 计算attr所属object的值
                Instruction* get_attr_instruction = new Instruction(InstructionType::GetAttr);

                size_t attr_name_id = NameMapper::get_instance().get_name_id(expression_node->token->value);
                get_attr_instruction->parameter = attr_name_id;

                code_object->instructions.push_back(get_attr_instruction);

                if (pop)
                {
                    Instruction* pop_top_instruction = new Instruction(InstructionType::PopTop);
                    code_object->instructions.push_back(pop_top_instruction);
                }
                return;
            }

            if (expression_node->type == SyntaxTreeNodeType::Call)
            {
                bool callee_is_method = expression_node->children[0]->type == SyntaxTreeNodeType::Attr;

                for (auto arg : expression_node->children[1]->children)  // expression_node->children[1]: Argument List
                {
                    this->__compile_expression(code_object, arg);
                }

                this->__compile_expression(code_object, expression_node->children[0]);  // 计算callee的值
                if (callee_is_method)  // 如果调用的是method, 则将最后的Get_Attr指令改为Get_Method
                {
                    code_object->instructions[code_object->instructions.size() - 1]->type = InstructionType::GetMethod;
                }

                Instruction* call_function_instruction = new Instruction(
                     callee_is_method ? InstructionType::CallMethod : InstructionType::CallFunction);
                call_function_instruction->parameter = expression_node->children[1]->children.size();
                code_object->instructions.push_back(call_function_instruction);

                if (pop)
                {
                    Instruction* pop_top_instruction = new Instruction(InstructionType::PopTop);
                    code_object->instructions.push_back(pop_top_instruction);
                }
                return;
            }

            if (expression_node->token->is_literal())
            {
                Instruction* load_literal_instruction = new Instruction(InstructionType::LoadLiteral);

                switch (expression_node->token->type)
                {
                case LexicalAnalysis::TokenType::IntLiteral:
                {
                    Builtin::IntValue int_value = std::stoll(expression_node->token->value);
                    if (!code_object->try_get_literal_index<Builtin::IntObject>(int_value, load_literal_instruction->parameter))
                    {
                        load_literal_instruction->parameter = code_object->register_literal_object(
                            XyA_Allocate(Builtin::IntObject, int_value));
                    }
                    break;
                }

                case LexicalAnalysis::TokenType::FloatLiteral:
                {
                    Builtin::FloatValue float_value = std::stod(expression_node->token->value);
                    if (!code_object->try_get_literal_index<Builtin::FloatObject>(float_value, load_literal_instruction->parameter))
                    {
                        load_literal_instruction->parameter = code_object->register_literal_object(
                            XyA_Allocate(Builtin::FloatObject, float_value));
                    }
                    break;
                }

                case LexicalAnalysis::TokenType::StringLiteral:
                {
                    Builtin::StringValue string_value;
                    string_value.assign(expression_node->token->value.begin() + 1, expression_node->token->value.end() - 1);

                    if (!code_object->try_get_literal_index<Builtin::StringObject>(string_value, load_literal_instruction->parameter))
                    {
                        Builtin::StringObject* str_obj = XyA_Allocate(Builtin::StringObject, string_value);
                        load_literal_instruction->parameter = code_object->register_literal_object(str_obj);
                    }
                    break;
                }

                case LexicalAnalysis::TokenType::BoolLiteral:
                {
                    Builtin::BoolObject* bool_obj = Builtin::BoolObject::get_instance(expression_node->token->value == "true");
                    load_literal_instruction->parameter = code_object->get_literal_index(bool_obj);
                    break;
                }

                case LexicalAnalysis::TokenType::NullLiteral:
                {
                    // TODO
                }

                default:
                    break;
                }

                code_object->instructions.push_back(load_literal_instruction);

                if (pop)
                {
                    Instruction* pop_top_instruction = new Instruction(InstructionType::PopTop);
                    code_object->instructions.push_back(pop_top_instruction);
                }
                return;
            }

            if (expression_node->token->type == LexicalAnalysis::TokenType::Identifier)
            {
                Instruction* load_variable_instruction = new Instruction(InstructionType::LoadVariable);

                if (!code_object->try_get_variable_index(expression_node->token->value, load_variable_instruction->parameter))
                {
                    load_variable_instruction->type = InstructionType::LoadGlobal;

                    if (!this->__global_code_object->try_get_variable_index(
                        expression_node->token->value, load_variable_instruction->parameter))
                    {
                        load_variable_instruction->parameter = 
                            this->__global_code_object->register_variable_name(expression_node->token->value);
                    }
                }

                code_object->instructions.push_back(load_variable_instruction);

                if (pop)
                {
                    Instruction* pop_top_instruction = new Instruction(InstructionType::PopTop);
                    code_object->instructions.push_back(pop_top_instruction);
                }
                return;
            }

            this->__compile_expression(code_object, expression_node->children[0]);
            if (expression_node->children.size() == 2)  // 二元运算符
            {
                this->__compile_expression(code_object, expression_node->children[1]);
            }

            Instruction* operation_instruction = new Instruction;
            switch (expression_node->token->type)
            {
            case LexicalAnalysis::TokenType::Op_Plus:
                operation_instruction->type = InstructionType::BinaryAdd;
                break;

            case LexicalAnalysis::TokenType::Op_Minus:
                operation_instruction->type = expression_node->children.size() == 2 ?
                    InstructionType::BinarySubtract : InstructionType::UnaryNegative;
                break;

            case LexicalAnalysis::TokenType::Op_Multiply:
                operation_instruction->type = InstructionType::BinaryMultiply;
                break;

            case LexicalAnalysis::TokenType::Op_Devide:
                operation_instruction->type = InstructionType::BinaryDevide;
                break;

            case LexicalAnalysis::TokenType::Op_Equal:
                operation_instruction->type = InstructionType::CompareIfEqual;
                break;

            case LexicalAnalysis::TokenType::Op_NotEqual:
                operation_instruction->type = InstructionType::CompareIfNotEqual;
                break;

            case LexicalAnalysis::TokenType::Op_GreaterThan:
                operation_instruction->type = InstructionType::CompareIfGreaterThan;
                break;

            case LexicalAnalysis::TokenType::Op_GreaterEqual:
                operation_instruction->type = InstructionType::CompareIfGreaterEqual;
                break;
            case LexicalAnalysis::TokenType::Op_LessThan:
                operation_instruction->type = InstructionType::CompareIfLessThan;
                break;

            case LexicalAnalysis::TokenType::Op_LessEqual:
                operation_instruction->type = InstructionType::CompareIfLessEqual;
                break;

            case LexicalAnalysis::TokenType::Op_Not:
                operation_instruction->type = InstructionType::UnaryNot;
                break;

            default:
                // TODO
                break;
            }

            code_object->instructions.push_back(operation_instruction);

            if (pop)
            {
                Instruction* pop_top_instruction = new Instruction(InstructionType::PopTop);
                code_object->instructions.push_back(pop_top_instruction);
            }

            return;
        }

        void Compiler::__compile_return(CodeObject* code_object, SyntaxTreeNode* return_node)
        {
            this->__compile_expression(code_object, return_node->children[0]);

            Instruction* return_instruction = new Instruction(InstructionType::Return);
            code_object->instructions.push_back(return_instruction);
        }

        CustomFunction* Compiler::__build_function(
            CodeObject* code_object, SyntaxTreeNode* function_definition_node, size_t& function_variable_index)
        {
            CustomFunction* function = XyA_Allocate_(CustomFunction);
            function->reference();

            if (code_object != nullptr &&
                !code_object->try_get_variable_index(function_definition_node->token->value, function_variable_index))
            {
                function_variable_index = code_object->register_variable_name(function_definition_node->token->value);
            }

            std::unordered_set<StringView> defined_argument_names;
            function->expected_arg_num = function_definition_node->children[0]->children.size();
            for (auto argument_definition_node : function_definition_node->children[0]->children)
            {
                StringView argument_name = argument_definition_node->token->value;
                if (defined_argument_names.find(argument_name) != defined_argument_names.end())
                {
                    this->__throw_error(
                        std::format("Duplicate argument '{}' found", argument_name), 
                        argument_definition_node->token->start_pos);
                    continue;
                }

                defined_argument_names.insert(argument_name);
                function->code_object->register_variable_name(argument_name);
            }

            this->__compile_block(function->code_object, function_definition_node->children[1]);

            return function;
        }

        std::optional<Attr> Compiler::__build_method(SyntaxTreeNode* method_definition_node, Type* cls)
        {
            auto inside_constructor_tem_setter = TemporarySetter(this->__inside_constructor, 
                method_definition_node->token->value == MagicMethodNames::init_method_name);
            auto inside_method_tem_setter = TemporarySetter(this->__inside_method, true);
            
            SyntaxTreeNode* argument_definition_node = method_definition_node->children[0];
            if (argument_definition_node->children.empty())
            {
                this->__throw_error(
                    "Methods must have a argument at position one as 'self'", 
                    argument_definition_node->token->start_pos);
                return std::nullopt;
            }
            this->__first_argument_name_of_method = argument_definition_node->children[0]->token->value;
            if (this->__first_argument_name_of_method != "self")
            {
                this->__send_message(
                    Message("'self' is a better name for the first argument of methods", MessageLevel::Warning),
                    argument_definition_node->children[0]->token->start_pos
                );
            }

            Attr attr;
            CustomFunction* method = XyA_Allocate_(CustomFunction);
            method->code_object->cls = cls;

            std::unordered_set<StringView> defined_argument_names;
            method->expected_arg_num = method_definition_node->children[0]->children.size();
            for (auto argument_definition_node : method_definition_node->children[0]->children)
            {
                StringView argument_name = argument_definition_node->token->value;
                if (defined_argument_names.find(argument_name) != defined_argument_names.end())
                {
                    this->__throw_error(
                        std::format("Duplicate argument '{}' found", argument_name), 
                        argument_definition_node->token->start_pos);
                    continue;
                }

                defined_argument_names.insert(argument_name);
                method->code_object->register_variable_name(argument_name);
            }

            this->__compile_block(method->code_object, method_definition_node->children[1]);
            attr.object = method;

            if (method_definition_node->children.size() == 3)
            {
                Token* modifier_token = method_definition_node->children[2]->token;
                switch (modifier_token->type)
                {
                case LexicalAnalysis::TokenType::Kw_Public:
                    attr.accessibility = AttrAccessibility::Public;
                    break;

                case LexicalAnalysis::TokenType::Kw_Private:
                    attr.accessibility = AttrAccessibility::Private;
                    break;

                default:
                    XyA_Throw_Core_Exception(
                        std::format("Invalid token '{}' as accessibility modifier", modifier_token->to_string()));
                }
            }
            else
            {
                    attr.accessibility = AttrAccessibility::Public;
            }

            return attr;
        }

        Object* cls_new_method(Object** args, size_t arg_num, bool& exception_thrown)
        {
            Type* cls = static_cast<Type*>(args[0]);
            Object* instance = XyA_Allocate_(Object);
            instance->reference();
            instance->set_type(cls);

            BaseFunction* init_method;
            AttrAccessibility accessibility;
            auto result = instance->try_get_method(MagicMethodNames::init_method_name_id, init_method, accessibility);

            if (accessibility == AttrAccessibility::Private && VirtualMachine::get_instance()->cur_context->cls() != cls)
            {
                exception_thrown = true;
                return XyA_Allocate(Builtin::BuiltinException, std::format("Can not access the private constructor of class {}", cls->name));
            }

            if (result == TryGetMethodResult::OK)
            {
                Object** init_Method_args = XyA_Allocate_Array_(Object*, arg_num);
                init_Method_args[0] = instance;
                for (size_t i = 1; i < arg_num; i ++)
                {
                    init_Method_args[i] = args[i];
                }
                Object* return_object = init_method->call(init_Method_args, arg_num, exception_thrown);

                if (exception_thrown)
                {
                    return return_object;
                }

                if (return_object->type() != Builtin::NullType::get_instance())
                {
                    exception_thrown = true;
                    return XyA_Allocate(Builtin::BuiltinException, "The return object of the method __init__ is not null");
                }

                XyA_Deallocate_Array(init_Method_args, arg_num);
            }
            instance->ref_count --;
            return instance;
        }

        Type* Compiler::__build_class(
            CodeObject* code_object, SyntaxTreeNode* class_definition_node, size_t& class_variable_index)
        {
            Type* cls = XyA_Allocate_(Type);
            cls->name = class_definition_node->token->value;
            cls->ref_count = 1;

            if (code_object != nullptr &&
                !code_object->try_get_variable_index(class_definition_node->token->value, class_variable_index))
            {
                class_variable_index = code_object->register_variable_name(class_definition_node->token->value);
            }

            for (auto class_definition_unit : class_definition_node->children)
            {
                if (class_definition_unit->type == SyntaxTreeNodeType::Method_Definition)
                {
                    StringView method_name = class_definition_unit->token->value;
                    Id method_name_id = NameMapper::get_instance().get_name_id(method_name);
    
                    auto method_building_result = this->__build_method(class_definition_unit, cls);
                    if (!method_building_result.has_value())
                    {
                        continue;
                    }
                    cls->set_attr(method_name_id, method_building_result.value());
                    this->__assignments_inside_constructor.clear();
                }
                else
                {
                    auto configuration_item_token = class_definition_unit->token;
                    auto configuration_value_token = class_definition_unit->children[0]->token;
                    if (configuration_item_token->value == "allow_ext_attr_add")
                    {
                        if (configuration_value_token->type != LexicalAnalysis::TokenType::BoolLiteral)
                        {
                            this->__throw_error(
                                "Expected a bool literal as the configuration value of configuration item 'allow_ext_attr_add'", 
                                configuration_value_token->start_pos
                            );
                            continue;
                        }
                        cls->allow_ext_attr_add = class_definition_unit->children[0]->token->value == "true" ? true : false;
                    }
                    else
                    {
                        this->__throw_error(
                            std::format("Unknown configuration item '{}'", configuration_item_token->value), 
                            configuration_item_token->start_pos
                        );
                        continue;
                    }
                }
            }
            Object* new_method = XyA_Allocate(Builtin::BuiltinFunction, cls_new_method);
            cls->set_attr(MagicMethodNames::new_method_name_id, new_method);

            return cls;
        }

        void Compiler::__throw_error(StringView message, LexicalAnalysis::TokenPos pos) const
        {
            this->__send_message(Message(message, MessageLevel::Error), pos);
        }

        void Compiler::__send_message(Message message, LexicalAnalysis::TokenPos pos) const
        {
            for (const auto& callback : this->__message_callbacks)
            {
                callback(message, pos);
            }
        }

    }  // namespace Compiler
}  // namespace XyA