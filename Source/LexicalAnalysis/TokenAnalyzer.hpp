#pragma once
#include <vector>
#include <functional>
#include <LexicalAnalysis/Token.hpp>
#include <Utils/StringUtils.hpp>


namespace XyA
{
    /* 词法分析相关命名空间 */
    namespace LexicalAnalysis
    {
        /* 词法分析器 */
        class TokenAnalyzer
        {
        public:
            /* 
            * 异常回调列表
            当在词法分析流程中发现了源码中的异常时, 调用异常回调列表中的所有回调

            回调函数类型: std::function<void(std::string_view, size_t)>
            回调函数参数: 
                std::string_view: 异常信息
                size_t: 异常位置
             */
            std::vector<std::function<void(std::string_view, size_t)>> exception_callbacks;

            /* 
            * 将源码分析为Token序列
            ! 注意: 返回值类型为std::vecotr<Token*>*, 在Token和Token Vector使用完毕后均需要在外部delete
            参数：
                (std::string_view) source: 源代码
            返回值:
                std::vector<Token*>* 经词法分析得到的Token序列
            */
            std::vector<Token*>* analyze_source(std::string_view source);

        private:
            std::string_view __analyzing_source;
            size_t __pos;
            bool __last_is_identifier;
            bool __finished;

            Token* __analyze_token();
            void __skip_space();
            void __skip_comment();

            bool __get_number_literal(std::string& result, bool& is_float);
            bool __get_string_literal(std::string& result);
            bool __get_identifier(std::string& result);

            bool __try_move_ptr(size_t step=1);
            bool __at_end() const;
            bool __match(std::string str, bool keyword_mode = false) const;
            char __cur_char() const;
            char __next_char() const;

            void __throw_exception(std::string_view message, size_t pos) const;
        };

        std::vector<Token*>* TokenAnalyzer::analyze_source(std::string_view source)
        {
            this->__analyzing_source = source;
            this->__pos = 0;
            this->__finished = false;

            // 在XyA.hpp的Core::execute中，当编译完成，tokens和其中的Token*会被释放
            std::vector<Token*>* tokens = new std::vector<Token*>;

            while (!this->__finished)
            {
                Token* token = this->__analyze_token();
                if (token != nullptr)
                {
                    tokens->push_back(token);
                }
            }

            return tokens;
        }

        Token* TokenAnalyzer::__analyze_token()
        {
            __skip_space();

            if (this->__match("//"))
            {
                this->__skip_comment();
                return nullptr;
            }

            if (this->__finished)
            {
                return nullptr;
            }

            Token* token = new Token;
            token->start_pos = this->__pos;

            // Separators
            if (this->__cur_char() == '(')
            {
                token->type = TokenType::S_LParenthesis;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }
            if (this->__cur_char() == ')')
            {
                token->type = TokenType::S_RParenthesis;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }
            if (this->__cur_char() == '{')
            {
                token->type = TokenType::S_LBrace;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }
            if (this->__cur_char() == '}')
            {
                token->type = TokenType::S_RBrace;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }
            if (this->__cur_char() == ';')
            {
                token->type = TokenType::S_Semicolon;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }
            if (this->__cur_char() == ',')
            {
                token->type = TokenType::S_Comma;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }

            // Operators
            if (this->__match("=="))
            {
                token->type = TokenType::Op_Equal;
                token->end_pos = this->__pos + 1;
                this->__try_move_ptr(2);
                return token;
            }
            if (this->__match("!="))
            {
                token->type = TokenType::Op_Equal;
                token->end_pos = this->__pos + 1;
                this->__try_move_ptr(2);
                return token;
            }
            if (this->__cur_char() == '=')
            {
                token->type = TokenType::Op_Assignment;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }
            if (this->__match(">="))
            {
                token->type = TokenType::Op_GreaterEqual;
                token->end_pos = this->__pos + 1;
                this->__try_move_ptr(2);
                return token;
            }
            if (this->__cur_char() == '>')
            {
                token->type = TokenType::Op_GreaterThan;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }
            if (this->__match("<="))
            {
                token->type = TokenType::Op_LessEqual;
                token->end_pos = this->__pos + 1;
                this->__try_move_ptr(2);
                return token;
            }
            if (this->__cur_char() == '<')
            {
                token->type = TokenType::Op_LessThan;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }
            if (this->__cur_char() == '+')
            {
                token->type = TokenType::Op_Plus;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }
            // 该分支会对负数字面量的负号进行识别
            // 由于编译期字面量折叠，不会影响运行性能
            if (this->__cur_char() == '-')
            {
                token->type = TokenType::Op_Minus;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }
            if (this->__cur_char() == '*')
            {
                token->type = TokenType::Op_Multiply;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }
            if (this->__cur_char() == '/')
            {
                token->type = TokenType::Op_Devide;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }
            if (this->__cur_char() == '.')
            {
                token->type = TokenType::Op_Dot;
                token->end_pos = this->__pos;
                this->__try_move_ptr();
                return token;
            }

            // Keywords
            if (this->__match("if", true))
            {
                token->type = TokenType::Kw_If;
                token->end_pos = this->__pos + 1;
                this->__try_move_ptr(2);
                return token;
            }
            if (this->__match("else", true))
            {
                token->type = TokenType::Kw_Else;
                token->end_pos = this->__pos + 3;
                this->__try_move_ptr(4);
                return token;
            }
            if (this->__match("for", true))
            {
                token->type = TokenType::Kw_For;
                token->end_pos = this->__pos + 2;
                this->__try_move_ptr(3);
                return token;
            }
            if (this->__match("while", true))
            {
                token->type = TokenType::Kw_While;
                token->end_pos = this->__pos + 4;
                this->__try_move_ptr(5);
                return token;
            }
            if (this->__match("fn", true))
            {
                token->type = TokenType::Kw_Fn;
                token->end_pos = this->__pos + 1;
                this->__try_move_ptr(2);
                return token;
            }
            if (this->__match("return", true))
            {
                token->type = TokenType::Kw_Return;
                token->end_pos = this->__pos + 5;
                this->__try_move_ptr(6);
                return token;
            }
            if (this->__match("class", true))
            {
                token->type = TokenType::Kw_Class;
                token->end_pos = this->__pos + 4;
                this->__try_move_ptr(5);
                return token;
            }
            if (this->__match("import", true))
            {
                token->type = TokenType::Kw_Import;
                token->end_pos = this->__pos + 5;
                this->__try_move_ptr(6);
                return token;
            }

            if (this->__match("true", true))
            {
                token->type = TokenType::BoolLiteral;
                token->value = "true";
                token->end_pos = this->__pos + 3;
                this->__try_move_ptr(4);
                return token;
            }
            if (this->__match("false", true))
            {
                token->type = TokenType::BoolLiteral;
                token->value = "false";
                token->end_pos = this->__pos + 4;
                this->__try_move_ptr(5);
                return token;
            }
            if (this->__match("null", true))
            {
                token->type = TokenType::NullLiteral;
                token->end_pos = this->__pos + 3;
                this->__try_move_ptr(4);
                return token;
            }
            if (Utils::is_digit(this->__cur_char()))  // Number Literal，由于负号已经被事先识别，所以只考虑首字符为数字
            {
                bool is_float = false;
                bool success = this->__get_number_literal(token->value, is_float);
                if (is_float)
                {
                    token->type = TokenType::FloatLiteral;
                }
                else
                {
                    token->type = TokenType::IntLiteral;
                }
                if (success)
                {
                    return token;
                }
                else
                {
                    this->__throw_exception("Invalid syntax", this->__pos);
                }
            }   
            
            if (this->__cur_char() == '"')  // String Literal
            {
                token->type = TokenType::StringLiteral;
                bool success = this->__get_string_literal(token->value);
                if (success)
                {
                    return token;
                }
                else
                {
                    this->__throw_exception("Invalid syntax", this->__pos);
                }
            }

            // Identifier    
            token->type = TokenType::Identifier;
            bool success = this->__get_identifier(token->value);
            if (success)
            {
                return token;
            }
            else
            {
                this->__throw_exception("Invalid syntax", this->__pos);
            }
        }

        void TokenAnalyzer::__skip_space()
        {
            while (this->__cur_char() == ' ' || this->__cur_char() == '\n')
            {
                if(!this->__try_move_ptr())
                {
                    return;
                }
            }
        }

        void TokenAnalyzer::__skip_comment()
        {
            while (this->__cur_char() != '\n')
            {
                if(!this->__try_move_ptr())
                {
                    return;
                }
            }
        }

        bool TokenAnalyzer::__get_number_literal(std::string& result, bool& is_float)
        {
            while(Utils::is_digit(this->__cur_char()) || this->__cur_char() == '.')
            {
                if (this->__cur_char() == '.')
                {
                    if (is_float)
                    {
                        this->__throw_exception("SyntaxError: Multiple decimal points found.", this->__pos);
                        return false;
                    }
                    is_float = true;
                }
                result.push_back(this->__cur_char());
                if (!this->__try_move_ptr())
                {
                    break;
                }
            }
            return true;
        }

        bool TokenAnalyzer::__get_string_literal(std::string& result)
        {
            if (!this->__try_move_ptr())  // 从左引号的下一个字符开始
            {
                this->__throw_exception("SyntaxError: '\"' was not closed", this->__pos);
                return false;
            }

            size_t firstCharPos = this->__pos;

            result.push_back('"');
            while (this->__cur_char() != '"')
            {  
                if (this->__cur_char() == '\n')
                {
                    break;
                }
                if (this->__cur_char() == '\\')
                {
                    if (!this->__at_end())
                    {
                        switch (this->__next_char())
                        {
                        case 'n':
                            result.push_back('\n');
                            this->__try_move_ptr();
                            break;

                        case '"':
                            result.push_back('"');
                            this->__try_move_ptr();
                            break;
                        
                        default:
                            result.push_back('\\');
                            break;
                        }
                    }
                    else
                    {
                        result.push_back('\\');
                    }
                }
                else
                {
                    result.push_back(this->__cur_char());
                }

                if (!this->__try_move_ptr())  // 从左引号的下一个字符开始
                {
                    this->__throw_exception("SyntaxError: '\"' was not closed", this->__pos);
                    return false;
                }
            }
            result.push_back('"');

            this->__try_move_ptr();
            return true;
        }

        bool TokenAnalyzer::__get_identifier(std::string& result)
        {
            if (!Utils::is_letter(this->__cur_char()) && this->__cur_char() != '_')
            {
                this->__throw_exception("SyntaxError: invalid syntax", this->__pos);
                return false;
            }
            while(Utils::is_letter(this->__cur_char()) || Utils::is_digit(this->__cur_char()) || this->__cur_char() == '_')
            {
                result += this->__cur_char();
                if (!this->__try_move_ptr())
                {
                    break;
                }
            }
            return true;
        }

        bool TokenAnalyzer::__try_move_ptr(size_t step)
        {
            if (this->__pos + step < this->__analyzing_source.length())
            {
                this->__pos += step;
                return true;
            }
            this->__finished = true;
            return false;
        }

        bool TokenAnalyzer::__at_end() const
        {
            return this->__pos == this->__analyzing_source.length() - 1;
        }

        bool TokenAnalyzer::__match(std::string str, bool keyword_mode) const
        {
            if (this->__pos + str.length() > this->__analyzing_source.length())
            {
                return false;
            }

            for (size_t i = 0; i < str.length(); i ++)
            {
                if (this->__analyzing_source[this->__pos + i] != str[i])
                {
                    return false;
                }
            }

            if(keyword_mode)
            {
                if (this->__pos + str.length() == this->__analyzing_source.length())
                {
                    return true;
                }
                else
                {
                    if (Utils::is_letter(this->__analyzing_source[this->__pos + str.length()]) || 
                        Utils::is_digit(this->__analyzing_source[this->__pos + str.length()]) || 
                        this->__analyzing_source[this->__pos + str.length()] == '_')
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        char TokenAnalyzer::__cur_char() const
        {
            return this->__analyzing_source[this->__pos];
        }

        char TokenAnalyzer::__next_char() const
        {
            return this->__analyzing_source[this->__pos + 1];
        }

        void TokenAnalyzer::__throw_exception(std::string_view message, size_t pos) const
        {
            for (auto callback : this->exception_callbacks)
            {
                callback(message, pos);
            }
        }
    }
}