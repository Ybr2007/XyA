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
            S_LBracket,                     // [
            S_RBracket,                     // ]
            S_LBrace,                       // {
            S_RBrace,                       // }
            S_Semicolon,                    // ;
            S_Comma,                        // ,
            S_Colon,                        // :
            S_VerticalBar,                  // |
            S_RArrow,                       // ->
            S_Hash,                         // #

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
            Kw_Return,                      // return
            Kw_Class,                       // class
            Kw_Import,                      // import
            Kw_Public,                      // public
            Kw_Private,                     // private
            Kw_As,                          // as

            /* Values */
            IntLiteral,                     // 整数字面量
            FloatLiteral,                   // 浮点数字面量
            StringLiteral,                  // 字符串字面量
            BoolLiteral,                    // 布尔字面量
            NullLiteral,                    // null字面量
            Identifier,                     // 标识符

            Unknown,                        // 占位
        };

        struct TokenPos
        {
            size_t row;
            size_t column;

            TokenPos& operator+(size_t offset);
        };

        /* Token, 程序源代码中可拆分的最小有意义单位 */
        class Token
        {
        public:
            TokenType type;     // Token的类型
            std::string value;  // Token的值, 只有在type为Literal或Indentifier时才有效
            TokenPos start_pos;
            TokenPos end_pos;

            std::string to_string() const;
            bool is_literal() const;
            bool is_visibility_modifier() const;
        };

    }  // namespace LexicalAnalysis
}  // namespace XyA