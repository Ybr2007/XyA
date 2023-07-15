#include <LexicalAnalysis/Token.h>


namespace XyA
{
    /* 词法分析相关命名空间 */
    namespace LexicalAnalysis
    {
        TokenPos& TokenPos::operator+(size_t offset)
        {
            this->column += offset;
            return *this;
        }

        std::string Token::to_string() const
        {
            switch (this->type)
            {
            case TokenType::S_LParenthesis:
                return "<Token: Separator '('>";
            case TokenType::S_RParenthesis:
                return "<Token: Separator ')'>";
            case TokenType::S_LBracket:
                return "<Token: Separator '['>";
            case TokenType::S_RBracket:
                return "<Token: Separator ']'>";
            case TokenType::S_LBrace:
                return "<Token: Separator '{'>";
            case TokenType::S_RBrace:
                return "<Token: Separator '}'>";
            case TokenType::S_Semicolon:
                return "<Token: Separator ';'>";
            case TokenType::S_Comma:
                return "<Token: Separator ','>";
            case TokenType::S_Colon:
                return "<Token: Separator ':'>";
            case TokenType::S_VerticalBar:
                return "<Token: Separator '|'>";
            case TokenType::S_RArrow:
                return "<Token: Separator '->'>";

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
            case TokenType::Kw_Return:
                return "<Token: Key Word 'return'>";
            case TokenType::Kw_Class:
                return "<Token: Key Word 'class'>";
            case TokenType::Kw_Import:
                return "<Token: Key Word 'import'>";
            case TokenType::Kw_Public:
                return "<Token: Key Word 'public'>";
            case TokenType::Kw_Private:
                return "<Token: Key Word 'private'>";
            case TokenType::Kw_As:
                return "<Token: Key Word 'as'>";

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

        bool Token::is_method_modifier() const
        {
            return this->type == TokenType::Kw_Public || this->type == TokenType::Kw_Private;
        }

    }  // namespace LexicalAnalysis
}  // namespace XyA