#pragma once
#include <functional>
#include <LexicalAnalysis/Token.hpp>
#include <SyntaxAnalysis/SyntaxTree.hpp>
#include <YJson/YJson.hpp>


namespace XyA
{
    namespace SyntaxAnalysis
    {
        class SyntaxParser
        {
        public:
            std::vector<std::function<void(std::string_view, LexicalAnalysis::TokenPos)>> exception_callbacks;

            SyntaxTreeNode* parse_tokens(std::vector<LexicalAnalysis::Token*>* tokens);

        private:
            size_t __cur_pos;
            bool __finished;
            std::vector<LexicalAnalysis::Token*>* __parsing_tokens;
            SyntaxTreeNode* __parsing_root;
            bool __inside_function;

            LexicalAnalysis::Token* __cur_token() const;

            /*
            * 获取当前指向的下一个Token
            不会改变__cur_pos的值
            ! 警告：该方法有越界风险，建议调用前使用if(!this->__at_end())进行检查
            */
            LexicalAnalysis::Token* __next_token() const;
            LexicalAnalysis::Token* __prev_token() const;
            bool __try_move_ptr(size_t step=1);
            void __backtrack(size_t step=1);
            bool __at_end() const;

            /*
            以下"__parse_"开头的方法对应着每一个语法元素
            在Source/SyntaxAnalysis/Grammar文件查看语法定义
            当这些方法被调用时，__cur_pos应指向要解析的语法元素的开头Token。当方法结束时，__cur_pos应指向语法元素的最后一个Token。
            默认第一个token已经被检验
            */

            SyntaxTreeNode* __parse_unit();
            SyntaxTreeNode* __parse_line();
            SyntaxTreeNode* __parse_block();
            SyntaxTreeNode* __parse_if();
            SyntaxTreeNode* __parse_while();
            SyntaxTreeNode* __parse_assignment(SyntaxTreeNode* assignment_target);
            SyntaxTreeNode* __parse_expression();
            SyntaxTreeNode* __parse_comparison();
            SyntaxTreeNode* __parse_addition();
            SyntaxTreeNode* __parse_multiplication();
            SyntaxTreeNode* __parse_unary();
            SyntaxTreeNode* __parse_primary();
            SyntaxTreeNode* __parse_call(SyntaxTreeNode* callee_expression);
            SyntaxTreeNode* __parse_call_argument_list();
            SyntaxTreeNode* __parse_attr(SyntaxTreeNode* attr_owner_expression);
            SyntaxTreeNode* __parse_function_definition();
            SyntaxTreeNode* __parse_argument_list_definition();
            SyntaxTreeNode* __parse_return();
            SyntaxTreeNode* __parse_method_definition();
            SyntaxTreeNode* __parse_class_definition();
            SyntaxTreeNode* __parse_type_conversion(SyntaxTreeNode* object_expression);

            void __throw_exception(std::string_view message, LexicalAnalysis::TokenPos pos) const;
        };

        SyntaxTreeNode* SyntaxParser::parse_tokens(std::vector<LexicalAnalysis::Token*>* tokens)
        {
            this->__cur_pos = 0;
            this->__finished = false;
            this->__parsing_tokens = tokens;
            this->__parsing_root = new SyntaxTreeNode(SyntaxTreeNodeType::Root);
            this->__inside_function = false;

            do
            {
                LexicalAnalysis::Token* cur_token = this->__cur_token();
                
                SyntaxTreeNode* node = this->__parse_unit();
                if (node != nullptr)
                {
                    this->__parsing_root->children.push_back(node);
                }
            }
            while (this->__try_move_ptr());

            return this->__parsing_root;
        }

        LexicalAnalysis::Token* SyntaxParser::__cur_token() const
        {
            return this->__parsing_tokens->at(this->__cur_pos);
        }

        LexicalAnalysis::Token* SyntaxParser::__next_token() const
        {
            return this->__parsing_tokens->at(this->__cur_pos + 1);
        }

        LexicalAnalysis::Token* SyntaxParser::__prev_token() const
        {
            return this->__cur_pos <= 0 ? 
                this->__parsing_tokens->at(0) : this->__parsing_tokens->at(this->__cur_pos - 1);
        }

        bool SyntaxParser::__try_move_ptr(size_t step)
        {
            if (this->__cur_pos + step < this->__parsing_tokens->size())
            {
                this->__cur_pos += step;
                return true;
            }
            this->__cur_pos = this->__parsing_tokens->size() - 1;
            this->__finished = true;
            return false;
        }

        void SyntaxParser::__backtrack(size_t step)
        {
            this->__cur_pos -= step;
        }

        bool SyntaxParser::__at_end() const
        {
            return this->__cur_pos >= this->__parsing_tokens->size() - 1;
        }

        SyntaxTreeNode* SyntaxParser::__parse_unit()
        {
            // unit -> block | if | line | function_definiton | class_definition
            switch (this->__cur_token()->type)
            {
            case LexicalAnalysis::TokenType::S_LBrace:
                return this->__parse_block();

            case LexicalAnalysis::TokenType::Kw_If:
                return this->__parse_if();

            case LexicalAnalysis::TokenType::Kw_While:
                return this->__parse_while();

            case LexicalAnalysis::TokenType::Kw_Fn:
                return this->__parse_function_definition();

            case LexicalAnalysis::TokenType::Kw_Class:
                return this->__parse_class_definition();

            case LexicalAnalysis::TokenType::Kw_Return:
                if (!this->__inside_function)
                {
                    this->__throw_exception("'return' outside functions", this->__cur_token()->start_pos);
                    return nullptr;
                }
                else
                {
                    return this->__parse_return();
                }
            
            default:
                return this->__parse_line();
            }
        }

        SyntaxTreeNode* SyntaxParser::__parse_line()
        {
            // line -> (assignment | expression) ";"
            SyntaxTreeNode* node = this->__parse_expression();
            if (node == nullptr)
            {
                return nullptr;
            }

            if (((node->type == SyntaxTreeNodeType::Primary && !node->token->is_literal()) || 
                node->type == SyntaxTreeNodeType::Attr) && !this->__at_end() && 
                this->__next_token()->type == LexicalAnalysis::TokenType::Op_Assignment)
            {
                node = this->__parse_assignment(node);
                if (node == nullptr)
                {
                    return nullptr;
                }
            }


            if (!this->__try_move_ptr() || this->__cur_token()->type != LexicalAnalysis::TokenType::S_Semicolon)
            {
                this->__throw_exception("Expected ';'", this->__cur_token()->start_pos);
                if (!this->__finished)
                {
                    this->__backtrack();
                }
                return nullptr;
            }

            return node;
        }

        SyntaxTreeNode* SyntaxParser::__parse_block()
        {
            // block -> "{" unit* "}"

            LexicalAnalysis::TokenPos left_brace_pos = this->__cur_token()->start_pos;
            if (!this->__try_move_ptr())
            {
                this->__throw_exception("'{' was not closed", left_brace_pos);
            }
            
            SyntaxTreeNode* node = new SyntaxTreeNode(SyntaxTreeNodeType::Block);

            while (this->__cur_token()->type != LexicalAnalysis::TokenType::S_RBrace)
            {
                SyntaxTreeNode* line_node = this->__parse_unit();
                if (line_node == nullptr)
                {
                    return nullptr;
                }

                node->children.push_back(line_node);

                if (!this->__try_move_ptr())
                {
                    this->__throw_exception("'{' was not closed", left_brace_pos);
                    return nullptr;
                }
            }

            return node;
        }

        SyntaxTreeNode* SyntaxParser::__parse_if()
        {
            // if -> "if" expression block ("else" (if | block))?
            if (!this->__try_move_ptr())
            {
                this->__throw_exception("Expected expression", this->__cur_token()->start_pos);
                return nullptr;
            }
            SyntaxTreeNode* expression = this->__parse_expression();
            if (expression == nullptr)
            {
                return nullptr;
            }

            if (!this->__try_move_ptr())
            {
                this->__throw_exception("Expected indented block", this->__cur_token()->start_pos);
                return nullptr;
            }
            SyntaxTreeNode* indented_block = this->__parse_block();
            if (indented_block == nullptr)
            {
                return nullptr;
            }

            SyntaxTreeNode* else_indented_block = nullptr;
            if (!this->__at_end() && this->__next_token()->type == LexicalAnalysis::TokenType::Kw_Else)
            {
                this->__try_move_ptr();  // 跳过"else"
                if(!this->__try_move_ptr())
                {
                    this->__throw_exception("Expected indented block", this->__cur_token()->start_pos);
                    return nullptr;
                }

                if (this->__cur_token()->type == LexicalAnalysis::TokenType::Kw_If)
                {
                    else_indented_block = this->__parse_if();
                }
                else if (this->__cur_token()->type == LexicalAnalysis::TokenType::S_LBrace)
                {
                    else_indented_block = this->__parse_block();
                }
                else
                {
                    this->__throw_exception("Expected indented block", this->__cur_token()->start_pos);
                    return nullptr;
                }

                if (else_indented_block == nullptr)
                {
                    return nullptr;
                }
            }

            SyntaxTreeNode* if_node = new SyntaxTreeNode(SyntaxTreeNodeType::If);
            if_node->children.reserve(else_indented_block == nullptr ? 2 : 3);
            if_node->children.push_back(expression);
            if_node->children.push_back(indented_block);
            if (else_indented_block != nullptr)
            {
                if_node->children.push_back(else_indented_block);
            }
            
            return if_node;
        }

        SyntaxTreeNode* SyntaxParser::__parse_while()
        {
            // while -> "while" expression block
            if (!this->__try_move_ptr())
            {
                this->__throw_exception("Expected expression", this->__cur_token()->start_pos);
                return nullptr;
            }
            SyntaxTreeNode* expression = this->__parse_expression();
            if (expression == nullptr)
            {
                return nullptr;
            }

            if (!this->__try_move_ptr())
            {
                this->__throw_exception("Expected indented block", this->__cur_token()->start_pos);
                return nullptr;
            }
            SyntaxTreeNode* indented_block = this->__parse_block();
            if (indented_block == nullptr)
            {
                return nullptr;
            }

            SyntaxTreeNode* if_node = new SyntaxTreeNode(SyntaxTreeNodeType::While);
            if_node->children.reserve(2);
            if_node->children.push_back(expression);
            if_node->children.push_back(indented_block);
            
            return if_node;
        }

        SyntaxTreeNode* SyntaxParser::__parse_assignment(SyntaxTreeNode* assignment_target)
        {
            // assignment -> assignable "=" expression
            if (!this->__try_move_ptr(2))
            {
                this->__throw_exception("Expected expression", this->__cur_token()->start_pos);
                return nullptr;
            }

            SyntaxTreeNode* expression = this->__parse_expression();
            if (expression == nullptr)
            {
                return nullptr;
            }

            SyntaxTreeNode* assignment = new SyntaxTreeNode(SyntaxTreeNodeType::Assignment);
            assignment->children.reserve(2);
            assignment->children.push_back(assignment_target);
            assignment->children.push_back(expression);
            return assignment;
        }

        SyntaxTreeNode* SyntaxParser::__parse_expression()
        {
            // expression -> comparison   
            return this->__parse_comparison();
        }    

        SyntaxTreeNode* SyntaxParser::__parse_comparison()
        {
            // comparison -> addition ( ( "!=" | "==" | ">" | ">=" | "<" | "<=" ) addition )?
            SyntaxTreeNode* left_addition = this->__parse_addition();
            if (left_addition == nullptr)
            {
                return nullptr;
            }

            if (this->__at_end())
            {
                return left_addition;
            }

            if (this->__next_token()->type == LexicalAnalysis::TokenType::Op_Equal ||
                this->__next_token()->type == LexicalAnalysis::TokenType::Op_NotEqual ||
                this->__next_token()->type == LexicalAnalysis::TokenType::Op_GreaterThan ||
                this->__next_token()->type == LexicalAnalysis::TokenType::Op_GreaterEqual ||
                this->__next_token()->type == LexicalAnalysis::TokenType::Op_LessThan ||
                this->__next_token()->type == LexicalAnalysis::TokenType::Op_LessEqual)
            {
                this->__try_move_ptr();
                LexicalAnalysis::Token* operator_token = this->__cur_token();

                if (!this->__try_move_ptr())
                {
                    this->__throw_exception("Expected a left addition", this->__cur_token()->start_pos);
                    return nullptr;
                }
                SyntaxTreeNode* right_addition = this->__parse_comparison();
                if (right_addition == nullptr)
                {
                    return nullptr;
                }

                SyntaxTreeNode* result_root = new SyntaxTreeNode(SyntaxTreeNodeType::Comparison);
                result_root->token = operator_token;
                result_root->children.push_back(left_addition);
                result_root->children.push_back(right_addition);
                return result_root;
            }
            return left_addition;
        }

        SyntaxTreeNode* SyntaxParser::__parse_addition()
        {
            // addition -> multiplication ( ( "-" | "+" ) multiplication )*
            SyntaxTreeNode* first_multiplication = this->__parse_multiplication();
            if (first_multiplication == nullptr)
            {
                return nullptr;
            }

            if (this->__at_end())
            {
                return first_multiplication;   
            }

            SyntaxTreeNode* cur_root = first_multiplication;
            SyntaxTreeNode* cur_left_multiplication = nullptr;

            while (this->__next_token()->type == LexicalAnalysis::TokenType::Op_Plus ||
                this->__next_token()->type == LexicalAnalysis::TokenType::Op_Minus)
            {
                this->__try_move_ptr();
                LexicalAnalysis::Token* operator_token = this->__cur_token();

                if (!this->__try_move_ptr())
                {
                    this->__throw_exception("Expected following multiplications", this->__cur_token()->start_pos);
                    return nullptr;
                }

                SyntaxTreeNode* following_multiplication = this->__parse_multiplication();
                if (following_multiplication == nullptr)
                {
                    return nullptr;
                }

                cur_left_multiplication = cur_root;

                cur_root = new SyntaxTreeNode(SyntaxTreeNodeType::Addition);
                cur_root->token = operator_token;
                cur_root->children.push_back(cur_left_multiplication);
                cur_root->children.push_back(following_multiplication);

                if (this->__at_end())
                {
                    return cur_root;
                }
            }

            return cur_root;
        }

        SyntaxTreeNode* SyntaxParser::__parse_multiplication()
        {
            // multiplication -> unary ( ( "*" | "/" ) unary )*
            SyntaxTreeNode* first_unary = this->__parse_unary();
            if (first_unary == nullptr)
            {
                return nullptr;
            }

            if (this->__at_end())
            {
                return first_unary;   
            }


            SyntaxTreeNode* cur_root = first_unary;
            SyntaxTreeNode* cur_left_unary = nullptr;

           while (this->__next_token()->type == LexicalAnalysis::TokenType::Op_Multiply ||
                this->__next_token()->type == LexicalAnalysis::TokenType::Op_Devide)
            {
                this->__try_move_ptr();
                LexicalAnalysis::Token* operator_token = this->__cur_token();

                if (!this->__try_move_ptr())
                {
                    this->__throw_exception("Expected following multiplications", this->__cur_token()->start_pos);
                    return nullptr;
                }

                SyntaxTreeNode* following_unary = this->__parse_unary();
                if (following_unary == nullptr)
                {
                    return nullptr;
                }

                cur_left_unary = cur_root;

                cur_root = new SyntaxTreeNode(SyntaxTreeNodeType::Multiplication);
                cur_root->token = operator_token;
                cur_root->children.push_back(cur_left_unary);
                cur_root->children.push_back(following_unary);

                if (this->__at_end())
                {
                    return cur_root;
                }
            }

            return cur_root;
        }

        SyntaxTreeNode* SyntaxParser::__parse_unary()
        {
            if (this->__cur_token()->type == LexicalAnalysis::TokenType::Op_Minus || 
                this->__cur_token()->type == LexicalAnalysis::TokenType::Op_Not)
            {
                SyntaxTreeNode* node = new SyntaxTreeNode(SyntaxTreeNodeType::Unary);
                node->token = this->__cur_token();
                if (!this->__try_move_ptr())
                {
                    this->__throw_exception("Expected expression", this->__cur_token()->start_pos);
                    return nullptr;
                }
                SyntaxTreeNode* primany = this->__parse_primary();
                if (primany == nullptr)
                {
                    return nullptr;
                }

                node->children.push_back(primany);
                return node;
            }

            return this->__parse_primary();
        }

        SyntaxTreeNode* SyntaxParser::__parse_primary()
        {
            // primary -> Literal | Identifier | ( "(" expression ")" ) | call | attr | type_conversion
            switch (this->__cur_token()->type)
            {
            case LexicalAnalysis::TokenType::IntLiteral:
            case LexicalAnalysis::TokenType::FloatLiteral:
            case LexicalAnalysis::TokenType::StringLiteral:
            case LexicalAnalysis::TokenType::BoolLiteral:
            case LexicalAnalysis::TokenType::NullLiteral:
            case LexicalAnalysis::TokenType::Identifier:
            {
                SyntaxTreeNode* node = new SyntaxTreeNode(SyntaxTreeNodeType::Primary);
                node->token = this->__cur_token();
                while (!this->__at_end())
                {
                    if (this->__next_token()->type == LexicalAnalysis::TokenType::S_LParenthesis)
                    {
                        node = this->__parse_call(node);
                    }
                    else if (this->__next_token()->type == LexicalAnalysis::TokenType::Op_Dot)
                    {
                        node = this->__parse_attr(node);
                    }
                    else if (this->__next_token()->type == LexicalAnalysis::TokenType::Kw_As)
                    {
                        node = this->__parse_type_conversion(node);
                    }
                    else
                    {
                        break;
                    }
                    
                    if (node == nullptr)
                    {
                        return nullptr;
                    }
                }
                return node;
            }
            
            case LexicalAnalysis::TokenType::S_LParenthesis:
            {
                if (!this->__try_move_ptr())
                {
                    this->__throw_exception("'(' was not closed", this->__cur_token()->start_pos);
                    return nullptr;
                }
                SyntaxTreeNode* expression = this->__parse_expression();
                if (expression == nullptr)
                {
                    return nullptr;
                }

                if (!this->__try_move_ptr() && 
                    this->__cur_token()->type != LexicalAnalysis::TokenType::S_RParenthesis)
                {
                    this->__throw_exception("'(' was not closed", this->__cur_token()->start_pos);
                    return nullptr;
                }
                while (!this->__at_end())
                {
                    if (this->__next_token()->type == LexicalAnalysis::TokenType::S_LParenthesis)
                    {
                        expression = this->__parse_call(expression);
                    }
                    else if (this->__next_token()->type == LexicalAnalysis::TokenType::Op_Dot)
                    {
                        expression = this->__parse_attr(expression);
                    }
                    else
                    {
                        break;
                    }

                    if (expression == nullptr)
                    {
                        return nullptr;
                    }
                }
                return expression;
            }

            default:
                this->__throw_exception("Expected expression", this->__cur_token()->start_pos);
                return nullptr;
            }  
        }   

        /*
        当前位置: 左括号前一个Token，并保证下一个Token是左括号
            [callee_expression]([arguments])
                              ^_
        */
        SyntaxTreeNode* SyntaxParser::__parse_call(SyntaxTreeNode* callee_expression)
        {
            SyntaxTreeNode* call_node = new SyntaxTreeNode(SyntaxTreeNodeType::Call);
            call_node->children.reserve(2);
            call_node->children.push_back(callee_expression);

            this->__try_move_ptr();
            if (this->__at_end())
            {
                this->__throw_exception("'(' was not closed", this->__cur_token()->start_pos);
                return nullptr;
            }
            
            SyntaxTreeNode* argument_list;
            if (this->__next_token()->type != LexicalAnalysis::TokenType::S_RParenthesis)
            {
                this->__try_move_ptr();
                argument_list = this->__parse_call_argument_list();

                if (argument_list == nullptr)
                {
                    return nullptr;
                }
            }
            else
            {
                argument_list = new SyntaxTreeNode(SyntaxTreeNodeType::Argument_List);
            }
            call_node->children.push_back(argument_list);


            if (!this->__try_move_ptr() || this->__cur_token()->type != LexicalAnalysis::TokenType::S_RParenthesis)
            {
                this->__throw_exception("'(' was not closed", this->__cur_token()->start_pos);
                return nullptr;
            }
            return call_node;
        }
    
        SyntaxTreeNode* SyntaxParser::__parse_call_argument_list()
        {
            // call_argument_list -> expression ("," expression)*
            SyntaxTreeNode* argument_list = new SyntaxTreeNode(SyntaxTreeNodeType::Argument_List);

            SyntaxTreeNode* first_argument_node = this->__parse_expression();
            if (first_argument_node == nullptr)
            {
                return nullptr;
            }

            argument_list->children.push_back(first_argument_node);

            if (!this->__try_move_ptr())
            {
                return argument_list;   
            }

            while (this->__cur_token()->type == LexicalAnalysis::TokenType::S_Comma)
            {
                if (!this->__try_move_ptr())
                {
                    this->__throw_exception("Expected argument", this->__cur_token()->start_pos);
                    return nullptr;
                }

                SyntaxTreeNode* following_argument_node = this->__parse_expression();
                if (following_argument_node == nullptr)
                {
                    return nullptr;
                }

                argument_list->children.push_back(following_argument_node);

                if (!this->__try_move_ptr())
                {
                    return argument_list;
                }
            }
            this->__cur_pos --;
            return argument_list;
        }

        /*
        当前位置: 点号前一个Token，并保证下一个Token是点号
            [object_expression].[attr_identifier]
                              ^_
        */
        SyntaxTreeNode* SyntaxParser::__parse_attr(SyntaxTreeNode* attr_owner_expression)
        {
            // attr -> expression "." Identifier
            SyntaxTreeNode* attr_node = new SyntaxTreeNode(SyntaxTreeNodeType::Attr);
            attr_node->children.reserve(1);
            attr_node->children.push_back(attr_owner_expression);

            this->__try_move_ptr();  // 已经确定下一个字符是点号，可以直接移动
            if (this->__at_end())
            {
                this->__throw_exception("Expected attribute identifier", this->__cur_token()->start_pos);
                return nullptr;
            }

            this->__try_move_ptr();  // 已知不是结尾，可以直接移动
            attr_node->token = this->__cur_token();

            return attr_node;
        }
        
        SyntaxTreeNode* SyntaxParser::__parse_function_definition()
        {
            // function_definition -> "fn" Identifier "(" argument_list_definition? ")" block
            this->__inside_function = true;

            if (this->__cur_token()->type != LexicalAnalysis::TokenType::Kw_Fn)
            {
                this->__throw_exception("Expected method definition", this->__cur_token()->start_pos);
                return nullptr;
            }

            if (!this->__try_move_ptr())
            {
                this->__throw_exception("Expected function name", this->__cur_token()->start_pos);
                return nullptr;
            }
            if (this->__cur_token()->type != LexicalAnalysis::TokenType::Identifier)
            {
                this->__throw_exception("Expected function name", this->__cur_token()->start_pos);
                return nullptr;
            }

            SyntaxTreeNode* function_defintion = new SyntaxTreeNode(SyntaxTreeNodeType::Function_Definition);
            function_defintion->token = this->__cur_token();

            if (!this->__try_move_ptr() || this->__cur_token()->type != LexicalAnalysis::TokenType::S_LParenthesis)
            {
                this->__throw_exception("Expected '('", this->__cur_token()->start_pos);
                return nullptr;
            }
            if (this->__at_end())
            {
                this->__throw_exception("'(' was not closed", this->__cur_token()->start_pos);
                return nullptr;
            }

            SyntaxTreeNode* argument_list;
            if (this->__next_token()->type != LexicalAnalysis::TokenType::S_RParenthesis)
            {
                this->__try_move_ptr();
                argument_list = this->__parse_argument_list_definition();

                if (argument_list == nullptr)
                {
                    return nullptr;
                }
            }
            else
            {
                argument_list = new SyntaxTreeNode(SyntaxTreeNodeType::Argument_List);
            }

            if (!this->__try_move_ptr())  // 移动到')'
            {
                this->__throw_exception("'(' was not closed", this->__cur_token()->start_pos);
                return nullptr;
            }
            if (!this->__try_move_ptr() || this->__cur_token()->type != LexicalAnalysis::TokenType::S_LBrace)
            {
                this->__throw_exception("Expected indented block ", this->__cur_token()->start_pos);
                return nullptr;
            }
            SyntaxTreeNode* function_body = this->__parse_block();
            if (function_body == nullptr)
            {
                return nullptr;
            }

            function_defintion->children.reserve(2);
            function_defintion->children.push_back(argument_list);
            function_defintion->children.push_back(function_body);

            this->__inside_function = false;

            return function_defintion;
        }

        SyntaxTreeNode* SyntaxParser::__parse_argument_list_definition()
        {
            // argument_list_definition -> Identifier ("," Identifier)*
            SyntaxTreeNode* argument_list = new SyntaxTreeNode(SyntaxTreeNodeType::Argument_List);

            if (this->__cur_token()->type != LexicalAnalysis::TokenType::Identifier)
            {
                this->__throw_exception("Expected identifiers as arguments", this->__cur_token()->start_pos);
                return nullptr;
            }

            LexicalAnalysis::Token* first_argument_token = this->__cur_token();
            SyntaxTreeNode* first_argument_node = new SyntaxTreeNode(SyntaxTreeNodeType::Argument);
            first_argument_node->token = first_argument_token;
            argument_list->children.push_back(first_argument_node);

            if (!this->__try_move_ptr())
            {
                return argument_list;   
            }

            while (this->__cur_token()->type == LexicalAnalysis::TokenType::S_Comma)
            {
                if (!this->__try_move_ptr() || this->__cur_token()->type != LexicalAnalysis::TokenType::Identifier)
                {
                    this->__throw_exception("Expected argument", this->__cur_token()->start_pos);
                    return nullptr;
                }

                LexicalAnalysis::Token* following_argument_token = this->__cur_token();
                SyntaxTreeNode* following_argument_node = new SyntaxTreeNode(SyntaxTreeNodeType::Argument);
                following_argument_node->token = following_argument_token;
                argument_list->children.push_back(following_argument_node);

                if (!this->__try_move_ptr())
                {
                    return argument_list;
                }
            }
            this->__cur_pos --;
            return argument_list;
        }

        SyntaxTreeNode* SyntaxParser::__parse_return()
        {
            // return -> "return" expression? ";"
            if (!this->__try_move_ptr())
            {
                this->__throw_exception("Expected expression", this->__cur_token()->start_pos);
                return nullptr;
            }
            
            SyntaxTreeNode* return_node = new SyntaxTreeNode(SyntaxTreeNodeType::Return);
            SyntaxTreeNode* expression = this->__parse_expression();
            if (expression == nullptr)
            {
                return nullptr;
            }
            
            return_node->children.reserve(1);
            return_node->children.push_back(expression);

            if (!this->__try_move_ptr() || this->__cur_token()->type != LexicalAnalysis::TokenType::S_Semicolon)
            {
                this->__throw_exception("Expected ';'", this->__cur_token()->start_pos);
                if (!this->__finished)
                {
                    this->__backtrack();
                }
                return nullptr;
            }

            return return_node;
        }

        SyntaxTreeNode* SyntaxParser::__parse_method_definition()
        {
            // method_definition -> VisibilityModifier? function_definition
            if (!this->__cur_token()->is_method_modifier())
            {
                SyntaxTreeNode* function_definition_node = this->__parse_function_definition();
                if (function_definition_node == nullptr)
                {
                    return nullptr;
                }

                function_definition_node->type = SyntaxTreeNodeType::Method_Definition;
                return function_definition_node;
            }

            SyntaxTreeNode* modifier_node = new SyntaxTreeNode(SyntaxTreeNodeType::Modifier);
            modifier_node->token = this->__cur_token();

            if (!this->__try_move_ptr())
            {
                this->__throw_exception("Expected method definition", this->__cur_token()->start_pos);
                return nullptr;
            }

            SyntaxTreeNode* function_definition_node = this->__parse_function_definition();
            if (function_definition_node == nullptr)
            {
                return nullptr;
            }
            function_definition_node->type = SyntaxTreeNodeType::Method_Definition;

            function_definition_node->children.reserve(3);
            function_definition_node->children.push_back(modifier_node);

            return function_definition_node;
        }

        SyntaxTreeNode* SyntaxParser::__parse_class_definition()
        {
            // class_definition -> "class" Identifier "{" method_definition* "}"
            // 调用时已确定当前token的type为Kw_Class
            
            if (!this->__try_move_ptr() || this->__cur_token()->type != LexicalAnalysis::TokenType::Identifier)
            {
                this->__throw_exception("Expected class name", this->__cur_token()->start_pos);
                return nullptr;
            }

            SyntaxTreeNode* class_definition_node = new SyntaxTreeNode(SyntaxTreeNodeType::Class_Definition);
            class_definition_node->token = this->__cur_token();

            if (!this->__try_move_ptr() || this->__cur_token()->type != LexicalAnalysis::TokenType::S_LBrace)
            {
                this->__throw_exception("Expected indented block", this->__cur_token()->start_pos);
                return nullptr;
            }
            LexicalAnalysis::TokenPos left_brace_pos = this->__cur_token()->start_pos;

            if (!this->__try_move_ptr())
            {
                this->__throw_exception("'{' was not closed", left_brace_pos);
                return nullptr;
            }
            while (this->__cur_token()->type != LexicalAnalysis::TokenType::S_RBrace)
            {
                SyntaxTreeNode* method_definition_node = this->__parse_method_definition();
                if (method_definition_node != nullptr)
                {
                    class_definition_node->children.push_back(method_definition_node);
                }

                if (!this->__try_move_ptr())
                {
                    this->__throw_exception("'{' was not closed", left_brace_pos);
                    return nullptr;
                }
            }

            return class_definition_node;
        }

        /* 当前情况：
            type_conversion -> expression "as" Identifier
                               ^^^^^^^^^^  ~~
         */
        SyntaxTreeNode* SyntaxParser::__parse_type_conversion(SyntaxTreeNode* object_expression)
        {
            // type_conversion -> expression "as" Identifier
            if (!this->__try_move_ptr(2) || this->__cur_token()->type != LexicalAnalysis::TokenType::Identifier)
            {
                this->__throw_exception("Expected the target type", this->__cur_token()->end_pos);
                return nullptr;
            }

            SyntaxTreeNode* type_conversion_node = new SyntaxTreeNode(SyntaxTreeNodeType::Type_Conversion);
            SyntaxTreeNode* target_type_node = new SyntaxTreeNode(SyntaxTreeNodeType::Primary);
            target_type_node->token = this->__cur_token();
            type_conversion_node->children.reserve(2);
            type_conversion_node->children.push_back(object_expression);
            type_conversion_node->children.push_back(target_type_node);

            return type_conversion_node;
        }

        void SyntaxParser::__throw_exception(std::string_view message, LexicalAnalysis::TokenPos pos) const
        {
            for (auto callback : this->exception_callbacks)
            {
                callback(message, pos);
            }
        }
    }  // namespace SyntaxAnalysis
}  // namespace XyA