#include <LexicalAnalysis/TokenAnalyzer.h>


namespace XyA
{
    /* 词法分析相关命名空间 */
    namespace LexicalAnalysis
    {
        std::vector<Token*>* TokenAnalyzer::analyze_source(std::string_view source)
        {
            this->__analyzing_source = source;
            this->__cur_char_ptr = 0;
            this->__cur_row = 1;
            this->__cur_column = 1;
            this->__finished = false;

            // 在XyA.hpp的Core::execute中, 当编译完成, tokens和其中的Token*会被释放
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
            token->start_pos = this->__cur_pos();

            // Separators
            if (this->__try_match_char_token(token, '(', TokenType::S_LParenthesis)) return token;
            if (this->__try_match_char_token(token, ')', TokenType::S_RParenthesis)) return token;
            if (this->__try_match_char_token(token, '[', TokenType::S_LBracket)) return token;
            if (this->__try_match_char_token(token, ']', TokenType::S_RBracket)) return token;
            if (this->__try_match_char_token(token, '{', TokenType::S_LBrace)) return token;
            if (this->__try_match_char_token(token, '}', TokenType::S_RBrace)) return token;
            if (this->__try_match_char_token(token, ';', TokenType::S_Semicolon)) return token;
            if (this->__try_match_char_token(token, ',', TokenType::S_Comma)) return token;
            if (this->__try_match_char_token(token, ':', TokenType::S_Colon)) return token;
            if (this->__try_match_char_token(token, '|', TokenType::S_VerticalBar)) return token;
            if (this->__try_match_string_token(token, "->", TokenType::S_RArrow)) return token;

            // Operators
            if (this->__try_match_string_token(token, "==", TokenType::Op_Equal)) return token;
            if (this->__try_match_string_token(token, "!=", TokenType::Op_NotEqual)) return token;
            if (this->__try_match_char_token(token, '=', TokenType::Op_Assignment)) return token;
            if (this->__try_match_string_token(token, ">=", TokenType::Op_GreaterEqual)) return token;
            if (this->__try_match_char_token(token, '>', TokenType::Op_GreaterThan)) return token;
            if (this->__try_match_string_token(token, "<=", TokenType::Op_LessEqual)) return token;
            if (this->__try_match_char_token(token, '<', TokenType::Op_LessThan)) return token;
            if (this->__try_match_char_token(token, '+', TokenType::Op_Plus)) return token;
            // 该分支会对负数字面量的负号进行识别
            // 由于编译期字面量折叠, 不会影响运行性能
            if (this->__try_match_char_token(token, '-', TokenType::Op_Minus)) return token;
            if (this->__try_match_char_token(token, '*', TokenType::Op_Multiply)) return token;
            if (this->__try_match_char_token(token, '/', TokenType::Op_Devide)) return token;
            if (this->__try_match_char_token(token, '.', TokenType::Op_Dot)) return token;

            // Keywords
            if (this->__try_match_string_token(token, "if", TokenType::Kw_If, true)) return token;
            if (this->__try_match_string_token(token, "else", TokenType::Kw_Else, true)) return token;
            if (this->__try_match_string_token(token, "for", TokenType::Kw_For, true)) return token;
            if (this->__try_match_string_token(token, "while", TokenType::Kw_While, true)) return token;
            if (this->__try_match_string_token(token, "fn", TokenType::Kw_Fn, true)) return token;
            if (this->__try_match_string_token(token, "return", TokenType::Kw_Return, true)) return token;
            if (this->__try_match_string_token(token, "class", TokenType::Kw_Class, true)) return token;
            if (this->__try_match_string_token(token, "import", TokenType::Kw_Import, true)) return token;
            if (this->__try_match_string_token(token, "public", TokenType::Kw_Public, true)) return token;
            if (this->__try_match_string_token(token, "private", TokenType::Kw_Private, true)) return token;
            if (this->__try_match_string_token(token, "as", TokenType::Kw_As, true)) return token;
            if (this->__try_match_string_token(token, "true", TokenType::BoolLiteral, true)) return token;
            if (this->__try_match_string_token(token, "false", TokenType::BoolLiteral, true)) return token;
            if (this->__try_match_string_token(token, "null", TokenType::NullLiteral, true)) return token;

            if (Utils::is_digit(this->__cur_char()))  // Number Literal, 由于负号已经被事先识别, 所以只考虑首字符为数字
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
                    token->end_pos = this->__cur_pos();
                    return token;
                }
                else
                {
                    delete token;
                    return nullptr;
                }
            }

            if (this->__cur_char() == '"')  // String Literal
            {
                token->type = TokenType::StringLiteral;
                bool success = this->__get_string_literal(token->value);
                if (success)
                {
                    token->end_pos = this->__cur_pos();
                    return token;
                }
                else
                {
                    delete token;
                    return nullptr;
                }
            }

            // Identifier
            token->type = TokenType::Identifier;
            bool success = this->__get_identifier(token->value);
            if (success)
            {
                token->end_pos = this->__cur_pos();
                return token;
            }
            else
            {
                delete token;
                return nullptr;
            }
        }

        bool TokenAnalyzer::__try_match_char_token(Token* token, char chr, TokenType token_type)
        {
            if (this->__cur_char() == chr)
            {
                token->type = token_type;
                token->end_pos = this->__cur_pos();
                this->__try_move_ptr();
                return true;
            }
            return false;
        }

        bool TokenAnalyzer::__try_match_string_token(Token* token, const std::string& str, TokenType token_type, bool keyword_mode)
        {
            if (this->__match(str, keyword_mode))
            {
                token->type = token_type;
                token->end_pos = this->__cur_pos() + (str.length() - 1);
                this->__try_move_ptr(str.length());
                return true;
            }
            return false;
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
                        this->__throw_exception("Multiple decimal points found.", this->__cur_pos());
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
                this->__throw_exception("'\"' was not closed", this->__cur_pos());
                return false;
            }

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
                    this->__throw_exception("'\"' was not closed", this->__cur_pos());
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
                this->__throw_exception("Invalid syntax", this->__cur_pos());
                while(!Utils::is_letter(this->__cur_char()) && this->__cur_char() != '_' && this->__try_move_ptr());
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
            if (this->__cur_char_ptr + step < this->__analyzing_source.length())
            {
                for (size_t i = 0; i < step; i++)
                {
                    if (this->__cur_char() == '\n')
                    {
                        this->__cur_row ++;
                        this->__cur_column = 1;
                    }
                    else
                    {
                        this->__cur_column ++;
                    }
                    this->__cur_char_ptr ++;
                }
                return true;
            }
            this->__finished = true;
            return false;
        }

        bool TokenAnalyzer::__at_end() const
        {
            return this->__cur_char_ptr == this->__analyzing_source.length() - 1;
        }

        bool TokenAnalyzer::__match(std::string str, bool keyword_mode) const
        {
            if (this->__cur_char_ptr + str.length() > this->__analyzing_source.length())
            {
                return false;
            }

            for (size_t i = 0; i < str.length(); i ++)
            {
                if (this->__analyzing_source[this->__cur_char_ptr + i] != str[i])
                {
                    return false;
                }
            }

            if(keyword_mode)
            {
                if (this->__cur_char_ptr + str.length() == this->__analyzing_source.length())
                {
                    return true;
                }
                else
                {
                    if (Utils::is_letter(this->__analyzing_source[this->__cur_char_ptr + str.length()]) ||
                        Utils::is_digit(this->__analyzing_source[this->__cur_char_ptr + str.length()]) ||
                        this->__analyzing_source[this->__cur_char_ptr + str.length()] == '_')
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        char TokenAnalyzer::__cur_char() const
        {
            return this->__analyzing_source[this->__cur_char_ptr];
        }

        char TokenAnalyzer::__next_char() const
        {
            return this->__analyzing_source[this->__cur_char_ptr + 1];
        }

        TokenPos TokenAnalyzer::__cur_pos() const
        {
            return TokenPos(this->__cur_row, this->__cur_column);
        }

        void TokenAnalyzer::__throw_exception(std::string_view message, TokenPos pos) const
        {
            for (auto callback : this->exception_callbacks)
            {
                callback(message, pos);
            }
        }
    }
}