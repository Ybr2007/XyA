#pragma once
#include <SyntaxAnalysis/SyntaxTree.hpp>
#include <Runtime/Instruction.h>
#include <Runtime/CodeObject.h>
#include <Runtime/Builtin/Int.h>
#include <Runtime/Builtin/String.h>
#include <Runtime/MemoryManager.hpp>
#include <Utils/StringUtils.hpp>


namespace XyA
{
    namespace Compiler
    {
        class Compiler
        {
        public:
            Runtime::CodeObject* compile(SyntaxAnalysis::SyntaxTreeNode* syntax_tree_root);

        private:
            Runtime::CodeObject* __compiling_code_obj;

            /* 
            在以下__compile_xxx中new的Instructions都会在Runtime::CodeObject::~CodeObject释放
            在以下__compile_xxx中new的Literals会被GC机制删除
             */
            void __compile_unit(SyntaxAnalysis::SyntaxTreeNode* unit_root);
            void __compile_line(SyntaxAnalysis::SyntaxTreeNode* line_root);
            void __compile_block(SyntaxAnalysis::SyntaxTreeNode* block_root);
            void __compile_if(SyntaxAnalysis::SyntaxTreeNode* if_root);
            void __compile_assignment(SyntaxAnalysis::SyntaxTreeNode* assignment_root);
            void __compile_expression(SyntaxAnalysis::SyntaxTreeNode* expression_root, bool pop=false);
        };

        Runtime::CodeObject* Compiler::compile(SyntaxAnalysis::SyntaxTreeNode* syntax_tree_root)
        {
            // delete于Runtime::Context::~Context
            this->__compiling_code_obj = new Runtime::CodeObject;

            this->__compiling_code_obj->add_variable_name("print");
            this->__compiling_code_obj->add_variable_name("_get_ref_count");
            this->__compiling_code_obj->add_variable_name("_get_id");
            
            for (auto unit : syntax_tree_root->children)
            {
                this->__compile_unit(unit);
            }

            return this->__compiling_code_obj;
        }

        void Compiler::__compile_unit(SyntaxAnalysis::SyntaxTreeNode* unit_root)
        {
            switch (unit_root->type)
            {
            case SyntaxAnalysis::SyntaxTreeNodeType::Block:
                this->__compile_block(unit_root);
                break;

            case SyntaxAnalysis::SyntaxTreeNodeType::If:
                this->__compile_if(unit_root);
                break;

            default:
                this->__compile_line(unit_root);
            }
        }

        void Compiler::__compile_block(SyntaxAnalysis::SyntaxTreeNode* block_root)
        {
            for (auto unit : block_root->children)
            {
                this->__compile_unit(unit);
            }
        }

        void Compiler::__compile_if(SyntaxAnalysis::SyntaxTreeNode* if_root)
        {
            this->__compile_expression(if_root->children[0]);

            Runtime::Instruction* jump_if_false_instruction = new Runtime::Instruction(Runtime::InstructionType::PopJumpIfFalse);
            this->__compiling_code_obj->instructions.push_back(jump_if_false_instruction);

            this->__compile_block(if_root->children[1]);
            jump_if_false_instruction->parameter = this->__compiling_code_obj->instructions.size();

            if (if_root->children.size() == 3)  // 有else语块
            {
                jump_if_false_instruction->parameter ++;

                Runtime::Instruction* jump_forward_instruction = new Runtime::Instruction(Runtime::InstructionType::JumpForward);
                this->__compiling_code_obj->instructions.push_back(jump_forward_instruction);

                if (if_root->children[2]->type == SyntaxAnalysis::SyntaxTreeNodeType::If)
                {
                    this->__compile_if(if_root->children[2]);
                }                
                else  // if_root->children[2]->type == SyntaxAnalysis::SyntaxTreeNodeType::Block
                {
                    this->__compile_block(if_root->children[2]);
                }

                jump_forward_instruction->parameter = this->__compiling_code_obj->instructions.size();
            }
        }

        void Compiler::__compile_line(SyntaxAnalysis::SyntaxTreeNode* line_root)
        {
            if (line_root->type == SyntaxAnalysis::SyntaxTreeNodeType::Assignment)
            {
                this->__compile_assignment(line_root);
                return;
            }
            if (line_root->is_expression())
            {
                this->__compile_expression(line_root, true);
                return;
            }
        }

        void Compiler::__compile_assignment(SyntaxAnalysis::SyntaxTreeNode* assignment_root)
        {
            this->__compile_expression(assignment_root->children[0]);

            Runtime::Instruction* store_variable_instruction = new Runtime::Instruction(Runtime::InstructionType::StroeVariable);;                

            if (!this->__compiling_code_obj->try_get_variable_index(assignment_root->token->value, store_variable_instruction->parameter))
            {
                store_variable_instruction->parameter = this->__compiling_code_obj->add_variable_name(assignment_root->token->value);
            }

            this->__compiling_code_obj->instructions.push_back(store_variable_instruction);
        }

        void Compiler::__compile_expression(SyntaxAnalysis::SyntaxTreeNode* expression_root, bool pop)
        {
            expression_root->try_fold_literal();

            if (expression_root->type == SyntaxAnalysis::SyntaxTreeNodeType::Call)
            {
                this->__compile_expression(expression_root->children[0]);  // 计算callee的值
                for (auto arg : expression_root->children[1]->children)  // expression_root->children[1]: Argument List
                {
                    this->__compile_expression(arg);
                }
                Runtime::Instruction* call_function_instruction = new Runtime::Instruction(Runtime::InstructionType::CallFunction);
                call_function_instruction->parameter = expression_root->children[1]->children.size();
                this->__compiling_code_obj->instructions.push_back(call_function_instruction);

                if (pop)
                {
                    Runtime::Instruction* pop_top_instruction = new Runtime::Instruction(Runtime::InstructionType::PopTop);
                    this->__compiling_code_obj->instructions.push_back(pop_top_instruction);
                }
                return;
            }

            if (expression_root->token->is_literal())
            {
                Runtime::Instruction* load_literal_instruction = new Runtime::Instruction(Runtime::InstructionType::LoadLiteral);

                switch (expression_root->token->type)
                {
                case LexicalAnalysis::TokenType::IntLiteral:
                {
                    Runtime::Builtin::IntObject* int_obj = Runtime::XyA_Allocate_(Runtime::Builtin::IntObject);
                    int_obj->value = std::stoll(expression_root->token->value);
                    if (!this->__compiling_code_obj->try_get_literal_index(int_obj, load_literal_instruction->parameter))
                    {
                        load_literal_instruction->parameter = this->__compiling_code_obj->add_literal_object(int_obj);
                    }
                    else  // 已经有了该对象，则需要释放掉多余的
                    {
                        Runtime::XyA_Deallocate(int_obj);
                    }
                    break;
                }

                case LexicalAnalysis::TokenType::FloatLiteral:
                {
                    Runtime::Builtin::FloatObject* float_obj = Runtime::XyA_Allocate_(Runtime::Builtin::FloatObject);
                    float_obj->value = std::stod(expression_root->token->value);
                    if (!this->__compiling_code_obj->try_get_literal_index(float_obj, load_literal_instruction->parameter))
                    {
                        load_literal_instruction->parameter = this->__compiling_code_obj->add_literal_object(float_obj);
                    }
                    else  // 已经有了该对象，则需要释放掉多余的
                    {
                        Runtime::XyA_Deallocate(float_obj);
                    }
                    break;
                }

                case LexicalAnalysis::TokenType::StringLiteral:
                {
                    Runtime::Builtin::StringObject* str_obj = Runtime::XyA_Allocate_(Runtime::Builtin::StringObject);
                    str_obj->value.assign(expression_root->token->value.begin() + 1, expression_root->token->value.end() - 1);
                    if (!this->__compiling_code_obj->try_get_literal_index(str_obj, load_literal_instruction->parameter))
                    {
                        load_literal_instruction->parameter = this->__compiling_code_obj->add_literal_object(str_obj);
                    }   
                    else  // 已经有了该对象，则需要释放掉多余的
                    {
                        Runtime::XyA_Deallocate(str_obj);
                    }
                    break;
                }

                case LexicalAnalysis::TokenType::BoolLiteral:
                {
                    Runtime::Builtin::BoolObject* bool_obj = Runtime::XyA_Allocate_(Runtime::Builtin::BoolObject);
                    bool_obj->value = expression_root->token->value == "true" ? true : false;
                    if (!this->__compiling_code_obj->try_get_literal_index(bool_obj, load_literal_instruction->parameter))
                    {
                        load_literal_instruction->parameter = this->__compiling_code_obj->add_literal_object(bool_obj);
                    }   
                    else  // 已经有了该对象，则需要释放掉多余的
                    {
                        Runtime::XyA_Deallocate(bool_obj);
                    }
                    break;
                }

                case LexicalAnalysis::TokenType::NullLiteral:
                {
                    // TODO   
                }
                
                default:
                    break;
                }

                this->__compiling_code_obj->instructions.push_back(load_literal_instruction);

                if (pop)
                {
                    Runtime::Instruction* pop_top_instruction = new Runtime::Instruction(Runtime::InstructionType::PopTop);
                    this->__compiling_code_obj->instructions.push_back(pop_top_instruction);
                }
                return;
            }

            if (expression_root->token->type == LexicalAnalysis::TokenType::Identifier)
            {
                Runtime::Instruction* load_variable_instruction = new Runtime::Instruction(Runtime::InstructionType::LoadVariable);          

                if (!this->__compiling_code_obj->try_get_variable_index(expression_root->token->value, load_variable_instruction->parameter))
                {
                    load_variable_instruction->parameter = this->__compiling_code_obj->add_variable_name(expression_root->token->value);
                }

                this->__compiling_code_obj->instructions.push_back(load_variable_instruction);

                if (pop)
                {
                    Runtime::Instruction* pop_top_instruction = new Runtime::Instruction(Runtime::InstructionType::PopTop);
                    this->__compiling_code_obj->instructions.push_back(pop_top_instruction);
                }
                return;
            }

            this->__compile_expression(expression_root->children[0]);
            if (expression_root->children.size() == 2)  // 二元运算符
            {
                this->__compile_expression(expression_root->children[1]);
            }

            Runtime::Instruction* operation_instruction = new Runtime::Instruction;
            switch (expression_root->token->type)
            {
            case LexicalAnalysis::TokenType::Op_Plus:
                operation_instruction->type = Runtime::InstructionType::BinaryAdd;
                break;

            case LexicalAnalysis::TokenType::Op_Minus:
                operation_instruction->type = expression_root->children.size() == 2 ? 
                    Runtime::InstructionType::BinarySubtract : Runtime::InstructionType::UnaryNegative;
                break;

            case LexicalAnalysis::TokenType::Op_Multiply:
                operation_instruction->type = Runtime::InstructionType::BinaryMultiply;
                break;

            case LexicalAnalysis::TokenType::Op_Devide:
                operation_instruction->type = Runtime::InstructionType::BinaryDevide;
                break;
            
            case LexicalAnalysis::TokenType::Op_Equal:
                operation_instruction->type = Runtime::InstructionType::CompareIfEqual;
                break;

            case LexicalAnalysis::TokenType::Op_NotEqual:
                operation_instruction->type = Runtime::InstructionType::CompareIfNotEqual;
                break;

            case LexicalAnalysis::TokenType::Op_GreaterThan:
                operation_instruction->type = Runtime::InstructionType::CompareIfGreaterThan;
                break;

            case LexicalAnalysis::TokenType::Op_GreaterEqual:
                operation_instruction->type = Runtime::InstructionType::CompareIfGreaterEqual;
                break;
            case LexicalAnalysis::TokenType::Op_LessThan:
                operation_instruction->type = Runtime::InstructionType::CompareIfLessThan;
                break;

            case LexicalAnalysis::TokenType::Op_LessEqual:
                operation_instruction->type = Runtime::InstructionType::CompareIfLessEqual;
                break;

            case LexicalAnalysis::TokenType::Op_Not:
                operation_instruction->type = Runtime::InstructionType::UnaryNot;
                break;
            
            default:
                // TODO
                break;
            }
            
            this->__compiling_code_obj->instructions.push_back(operation_instruction);

            if (pop)
            {
                Runtime::Instruction* pop_top_instruction = new Runtime::Instruction(Runtime::InstructionType::PopTop);
                this->__compiling_code_obj->instructions.push_back(pop_top_instruction);
            }

            return;
        }

    }  // namespace Compiler
}  // namespace XyA