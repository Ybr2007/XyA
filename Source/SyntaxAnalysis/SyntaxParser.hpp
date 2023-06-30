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
            std::vector<std::function<void(std::string_view, size_t)>> exception_callbacks;

            SyntaxTreeNode* parse_tokens(std::vector<LexicalAnalysis::Token*>* tokens);

        private:
            size_t __cur_pos;
            bool __finished;
            std::vector<LexicalAnalysis::Token*>* __parsing_tokens;
            SyntaxTreeNode* __parsing_root;

            LexicalAnalysis::Token* __cur_token() const;

            /*
            * 获取当前指向的下一个Token
            不会改变__cur_pos的值
            ! 警告：该方法有越界风险，建议调用前使用if(!this->__at_end())进行检查
            */
            LexicalAnalysis::Token* __next_token() const;
            bool __try_move_ptr(size_t step=1);
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
            SyntaxTreeNode* __parse_assignment();
            SyntaxTreeNode* __parse_expression();
            SyntaxTreeNode* __parse_comparison();
            SyntaxTreeNode* __parse_addition();
            SyntaxTreeNode* __parse_multiplication();
            SyntaxTreeNode* __parse_unary();
            SyntaxTreeNode* __parse_primary();
            SyntaxTreeNode* __parse_call(SyntaxTreeNode* callee);
            SyntaxTreeNode* __parse_call_argument_list();
            SyntaxTreeNode* __parse_function_definition();
            SyntaxTreeNode* __parse_argument_list_definition();

            void __throw_exception(std::string_view message, size_t pos) const;
        };

        SyntaxTreeNode* SyntaxParser::parse_tokens(std::vector<LexicalAnalysis::Token*>* tokens)
        {
            this->__cur_pos = 0;
            this->__finished = false;
            this->__parsing_tokens = tokens;
            this->__parsing_root = new SyntaxTreeNode(SyntaxTreeNodeType::Root);

            do
            {
                LexicalAnalysis::Token* cur_token = this->__cur_token();
                
                SyntaxTreeNode* node = this->__parse_unit();
                this->__parsing_root->children.push_back(node);
            }
            while (this->__try_move_ptr());

            return this->__parsing_root;
        }

        LexicalAnalysis::Token* SyntaxParser::__cur_token() const
        {
            return this->__parsing_tokens->operator[](this->__cur_pos);
        }

        LexicalAnalysis::Token* SyntaxParser::__next_token() const
        {
            return this->__parsing_tokens->operator[](this->__cur_pos + 1);
        }

        bool SyntaxParser::__try_move_ptr(size_t step)
        {
            if (this->__cur_pos + step < this->__parsing_tokens->size())
            {
                this->__cur_pos += step;
                return true;
            }
            this->__finished = true;
            return false;
        }

        bool SyntaxParser::__at_end() const
        {
            return this->__cur_pos >= this->__parsing_tokens->size() - 1;
        }

        SyntaxTreeNode* SyntaxParser::__parse_unit()
        {
            // unit -> block | if | line | function_definiton
            if (this->__cur_token()->type == LexicalAnalysis::TokenType::S_LBrace)
            {
                return this->__parse_block();
            }
            else if (this->__cur_token()->type == LexicalAnalysis::TokenType::Kw_If)
            {
                return this->__parse_if();
            }
            else if (this->__cur_token()->type == LexicalAnalysis::TokenType::Kw_Fn)
            {
                return this->__parse_function_definition();
            }
            else
            {
                return this->__parse_line();
            }
        }

        SyntaxTreeNode* SyntaxParser::__parse_line()
        {
            // line -> (assignment | expression) ";"
            SyntaxTreeNode* node;
            
            if (this->__cur_token()->type == LexicalAnalysis::TokenType::Identifier && 
                !this->__at_end() && this->__next_token()->type == LexicalAnalysis::TokenType::Op_Assignment)
            {
                node = this->__parse_assignment();
            }
            else
            {
                node = this->__parse_expression();
            }

            if (!this->__try_move_ptr() || this->__cur_token()->type != LexicalAnalysis::TokenType::S_Semicolon)
            {
                this->__throw_exception("Expected ';'", this->__cur_token()->start_pos);
            }
            
            return node;
        }

        SyntaxTreeNode* SyntaxParser::__parse_block()
        {
            // block -> "{" unit* "}"

            size_t left_brace_pos = this->__cur_pos;
            if (!this->__try_move_ptr())
            {
                this->__throw_exception("'{' was not closed", left_brace_pos);
            }
            
            SyntaxTreeNode* node = new SyntaxTreeNode(SyntaxTreeNodeType::Block);

            while (this->__cur_token()->type != LexicalAnalysis::TokenType::S_RBrace)
            {
                node->children.push_back(this->__parse_unit());

                if (!this->__try_move_ptr())
                {
                    this->__throw_exception("'{' was not closed", left_brace_pos);
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
            }
            SyntaxTreeNode* expression = this->__parse_expression();

            if (!this->__try_move_ptr())
            {
                this->__throw_exception("Expected indented block", this->__cur_token()->start_pos);
            }
            SyntaxTreeNode* indented_block = this->__parse_block();

            SyntaxTreeNode* else_indented_block = nullptr;
            if (!this->__at_end() && this->__next_token()->type == LexicalAnalysis::TokenType::Kw_Else)
            {
                this->__try_move_ptr();  // 跳过"else"
                if(!this->__try_move_ptr())
                {
                    this->__throw_exception("Expected indented block", this->__cur_token()->start_pos);
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

        SyntaxTreeNode* SyntaxParser::__parse_assignment()
        {
            // assignment -> Identifier "=" expression
            LexicalAnalysis::Token* target_identifier = this->__cur_token();

            if (!this->__try_move_ptr(2))
            {
                this->__throw_exception("Expected expression", this->__cur_token()->start_pos);
            }

            SyntaxTreeNode* expression = this->__parse_expression();

            SyntaxTreeNode* assignment = new SyntaxTreeNode(SyntaxTreeNodeType::Assignment);
            assignment->token = target_identifier;
            assignment->children.reserve(1);
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
                }
                SyntaxTreeNode* right_addition = this->__parse_comparison();

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
                }

                SyntaxTreeNode* following_multiplication = this->__parse_multiplication();

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
                }

                SyntaxTreeNode* following_unary = this->__parse_unary();

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
                }
                SyntaxTreeNode* primany = this->__parse_primary();
                node->children.push_back(primany);
                return node;
            }

            return this->__parse_primary();
        }

        SyntaxTreeNode* SyntaxParser::__parse_primary()
        {
            // primary -> Literal | Identifier | "(" expression ")" | call
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
                while (!this->__at_end() && this->__next_token()->type == LexicalAnalysis::TokenType::S_LParenthesis)
                {
                    node = this->__parse_call(node);
                }
                return node;
            }
            
            case LexicalAnalysis::TokenType::S_LParenthesis:
            {
                if (!this->__try_move_ptr())
                {
                    this->__throw_exception("'(' was not closed", this->__cur_token()->start_pos);
                }
                SyntaxTreeNode* expression = this->__parse_expression();
                if (!this->__try_move_ptr() && 
                    this->__cur_token()->type != LexicalAnalysis::TokenType::S_RParenthesis)
                {
                    this->__throw_exception("'(' was not closed", this->__cur_token()->start_pos);
                }
                while (!this->__at_end() && this->__next_token()->type == LexicalAnalysis::TokenType::S_LParenthesis)
                {
                    expression = this->__parse_call(expression);
                }
                return expression;
            }

            default:
                this->__throw_exception("Expected expression", this->__cur_token()->start_pos);
            }  
        }   

        SyntaxTreeNode* SyntaxParser::__parse_call(SyntaxTreeNode* callee)
        {
            SyntaxTreeNode* call_node = new SyntaxTreeNode(SyntaxTreeNodeType::Call);
            call_node->children.reserve(2);
            call_node->children.push_back(callee);

            this->__try_move_ptr();
            if (this->__at_end())
            {
                this->__throw_exception("'(' was not closed", this->__cur_token()->start_pos);
            }
            
            SyntaxTreeNode* argument_list;
            if (this->__next_token()->type != LexicalAnalysis::TokenType::S_RParenthesis)
            {
                this->__try_move_ptr();
                argument_list = this->__parse_call_argument_list();
            }
            else
            {
                argument_list = new SyntaxTreeNode(SyntaxTreeNodeType::Argument_List);
            }
            call_node->children.push_back(argument_list);


            if (!this->__try_move_ptr() || this->__cur_token()->type != LexicalAnalysis::TokenType::S_RParenthesis)
            {
                this->__throw_exception("'(' was not closed", this->__cur_token()->start_pos);
            }
            return call_node;
        }
    
        SyntaxTreeNode* SyntaxParser::__parse_call_argument_list()
        {
            // call_argument_list -> expression ("," expression)*
            SyntaxTreeNode* argument_list = new SyntaxTreeNode(SyntaxTreeNodeType::Argument_List);

            SyntaxTreeNode* first_argument_node = this->__parse_expression();
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
                }

                SyntaxTreeNode* following_argument_node = this->__parse_expression();
                argument_list->children.push_back(following_argument_node);

                if (!this->__try_move_ptr())
                {
                    return argument_list;
                }
            }
            this->__cur_pos --;
            return argument_list;
        }
        
        SyntaxTreeNode* SyntaxParser::__parse_function_definition()
        {
            // function_definition -> "fn" Identifier "(" argument_list_definition? ")" block
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

            function_defintion->children.reserve(2);
            function_defintion->children.push_back(argument_list);
            function_defintion->children.push_back(function_body);
            return function_defintion;
        }

        SyntaxTreeNode* SyntaxParser::__parse_argument_list_definition()
        {
            // argument_list_definition -> Identifier ("," Identifier)*
            SyntaxTreeNode* argument_list = new SyntaxTreeNode(SyntaxTreeNodeType::Argument_List);

            if (this->__cur_token()->type != LexicalAnalysis::TokenType::Identifier)
            {
                this->__throw_exception("Expected identifiers as arguments", this->__cur_token()->start_pos);
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

        void SyntaxParser::__throw_exception(std::string_view message, size_t pos) const
        {
            for (auto callback : this->exception_callbacks)
            {
                callback(message, pos);
            }
        }
    }  // namespace SyntaxAnalysis
}  // namespace XyA