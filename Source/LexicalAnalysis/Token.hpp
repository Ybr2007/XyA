#pragma once
#include <string>


namespace XyA
{
    /* 词法分析相关命名空间 */
    namespace LexicalAnalysis
    {
        /* 
        * Token类型
        分为: 分隔符(Separator), 操作符(Operator), 关键字(Keyword), 值(Value) 几大类
        分隔符类型名称以"S_"开头, 操作符类型名称以"Op_"开头, 关键字类型名称以"Kw_"开头
        */
        enum class TokenType
        {
            /* Separators */
            S_LParenthesis,                 // (
            S_RParenthesis,                 // )
            S_LBrace,                       // {
            S_RBrace,                       // }
            S_Semicolon,                    // ;
            S_Comma,                        // ,

            /* Operators */
            Op_Assignment,                  // =
            Op_Equal,                       // ==
            Op_NotEqual,                    // !=
            Op_GreaterThan,                 // >
            Op_GreaterEqual,                // >=
            Op_LessThan,                    // <
            Op_LessEqual,                   // <=
            Op_Not,                         // !
            Op_Plus,                        // +
            Op_Minus,                       // -
            Op_Multiply,                    // *
            Op_Devide,                      // /
            Op_Dot,                         // .            

            /* Keywords */
            Kw_If,                          // if
            Kw_Else,                        // else
            Kw_For,                         // for
            Kw_While,                       // while
            Kw_Fn,                          // fn
            Kw_Import,                      // import

            /* Values */
            IntLiteral,                     // 整数字面量
            FloatLiteral,                   // 浮点数字面量
            StringLiteral,                  // 字符串字面量
            BoolLiteral,                    // 布尔字面量
            NullLiteral,                    // null字面量
            Identifier,                     // 标识符

            Unknown,                        // 占位
        };

        /* Token, 程序源代码中可拆分的最小有意义单位 */
        class Token
        {
        public:
            TokenType type;     // Token的类型
            std::string value;  // Token的值，只有在type为Literal或Indentifier时才有效
            size_t start_pos;   // Token在源码中开始的位置
            size_t end_pos;     // Token再源码中结束的位置

            std::string to_string() const;
            bool is_literal() const;
        };

        std::string Token::to_string() const
        {
            switch (this->type)
            {
            case TokenType::S_LParenthesis:
                return "<Token: Separator '('>";
            case TokenType::S_RParenthesis:
                return "<Token: Separator ')'>";
            case TokenType::S_LBrace:
                return "<Token: Separator '{'>";
            case TokenType::S_RBrace:
                return "<Token: Separator '}'>";
            case TokenType::S_Semicolon:
                return "<Token: Separator ';'>";
            case TokenType::S_Comma:
                return "<Token: Separator ','>";

            case TokenType::Op_Assignment:
                return "<Token: Operator '='>";
            case TokenType::Op_Equal:
                return "<Token: Operator '=='>";
            case TokenType::Op_NotEqual:
                return "<Token: Operator '!='>";
            case TokenType::Op_GreaterThan:
                return "<Token: Operator '>'>";
            case TokenType::Op_GreaterEqual:
                return "<Token: Operator '>='>";
            case TokenType::Op_LessThan:
                return "<Token: Operator '<'>";
            case TokenType::Op_LessEqual:
                return "<Token: Operator '<='>";
            case TokenType::Op_Plus:
                return "<Token: Operator '+'>";
            case TokenType::Op_Minus:
                return "<Token: Operator '-'>";
            case TokenType::Op_Multiply:
                return "<Token: Operator '*'>";
            case TokenType::Op_Devide:
                return "<Token: Operator '/'>";
            case TokenType::Op_Dot:
                return "<Token: Operator '.'>";
            
                
            case TokenType::Kw_If:
                return "<Token: Key Word 'if'>";
            case TokenType::Kw_Else:
                return "<Token: Key Word 'else'>";
            case TokenType::Kw_For:
                return "<Token: Key Word 'for'>";
            case TokenType::Kw_While:
                return "<Token: Key Word 'while'>";
            case TokenType::Kw_Fn:
                return "<Token: Key Word 'fn'>";
            case TokenType::Kw_Import:
                return "<Token: Key Word 'import'>";

            case TokenType::IntLiteral:
                return "<Token: Int Literal, Value: '" + value +"'>";
            case TokenType::FloatLiteral:
                return "<Token: Float Literal, Value: '" + value +"'>";  
            case TokenType::StringLiteral:
                return "<Token: String Literal, Value: '" + value +"'>";  
            case TokenType::BoolLiteral:
                return "<Token: Bool Literal, Value: '" + value +"'>"; 
            case TokenType::NullLiteral:
                return "<Token: Null Literal>";            
            case TokenType::Identifier:
                return "<Token: Identifier, Value: '" + value +"'>";
            
            default:
                return "<Unknown Token>";
            }
        }

        bool Token::is_literal() const
        {
            return this->type == TokenType::IntLiteral || this->type == TokenType::FloatLiteral || 
                this->type == TokenType::StringLiteral || this->type == TokenType::BoolLiteral ||
                this->type == TokenType::NullLiteral;
        }
    }  // namespace LexicalAnalysis
}  // namespace XyA