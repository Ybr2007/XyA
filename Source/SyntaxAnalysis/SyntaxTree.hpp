#pragma once
#include <vector>
#include <sstream>
#include <LexicalAnalysis/Token.hpp>
#include <Config.h>
#ifdef Debug_Write_AST_To_Json_File
#include <YJson/YJson.hpp>
#endif

namespace XyA
{
    namespace SyntaxAnalysis
    {
        enum class SyntaxTreeNodeType
        {
            Root,                   // children: 数量不限，每个child是一个语句（块）
            Block,                  // children: 数量不限，每个child是一个语句（块）
            If,                     // children: 数量为2，children[0] 是判断条件表达式，children[1] 是如果为真执行的语句块
            Assignment,             // value: 赋值目标 children: 数量为1，值的表达式
            Comparison,             // value: 运算符 chilren: 数量为2，为两个运算数
            Addition,               // value: 运算符 chilren: 数量为2，为两个运算数
            Multiplication,         // value: 运算符 chilren: 数量为2，为两个运算数
            Unary,                  // value: 运算符 chilren: 数量为1，为运算数
            Primary,                // value: 标识符或字面量
            Function_Definition,    // value: 函数名称标识符 children: 数量为2，children[0] 形参列表(Argument_List)，children[1] 函数体(Block)
            Argument_List,          // children: 数量不限，每一个child是一个参数（对于Definition_Argument_List，是形参标识符；对于Call_Argument_List，是实参表达式）
            Argument,               // value: 形参名称标识符
            Call,                   // chilren: 数量为2，children[0] 为callee表达式，children[1] 为实参列表(Argument_List)
            Return,                 // children: 数量为1，chilren[0] 为返回值表达式
        };

        #ifdef Debug_Write_AST_To_Json_File
        std::string to_string(SyntaxTreeNodeType type)
        {
            switch (type)
            {
            case SyntaxTreeNodeType::Root:
                return "<SyntaxTreeNodeType: Root>";
            case SyntaxTreeNodeType::Block:
                return "<SyntaxTreeNodeType: Block>";
            case SyntaxTreeNodeType::If:
                return "<SyntaxTreeNodeType: If>";
            case SyntaxTreeNodeType::Assignment:
                return "<SyntaxTreeNodeType: Assignment>";
            case SyntaxTreeNodeType::Comparison:
                return "<SyntaxTreeNodeType: Comparison>";
            case SyntaxTreeNodeType::Addition:
                return "<SyntaxTreeNodeType: Addition>";
            case SyntaxTreeNodeType::Multiplication:
                return "<SyntaxTreeNodeType: Multiplication>";
            case SyntaxTreeNodeType::Unary:
                return "<SyntaxTreeNodeType: Unary>";
            case SyntaxTreeNodeType::Primary:
                return "<SyntaxTreeNodeType: Primary>";
            case SyntaxTreeNodeType::Function_Definition:
                return "<SyntaxTreeNodeType: Function_Definition>";
            case SyntaxTreeNodeType::Argument_List:
                return "<SyntaxTreeNodeType: Argument_List>";
            case SyntaxTreeNodeType::Argument:
                return "<SyntaxTreeNodeType: Argument>";
            case SyntaxTreeNodeType::Call:
                return "<SyntaxTreeNodeType: Call>";
            default:
                return "<Unknown>";
            }
        }
        #endif

        class SyntaxTreeNode
        {
        public:
            SyntaxTreeNodeType type;
            LexicalAnalysis::Token* token = nullptr;
            std::vector<SyntaxTreeNode*> children;

            SyntaxTreeNode(SyntaxTreeNodeType type);
            ~SyntaxTreeNode();

            bool is_expression() const;

            /*
            * 尝试进行常量折叠
            ! 警告：不会检查node类型，调用前确定is_expression()为true
            */
            bool try_fold_literal();
            
            #ifdef Debug_Write_AST_To_Json_File
            YJson::Object* to_json() const;
            #endif

        private:
            bool __flod_number_literals();
        };

        SyntaxTreeNode::SyntaxTreeNode(SyntaxTreeNodeType type)
        {
            this->type = type;
        }

        SyntaxTreeNode::~SyntaxTreeNode()
        {
            for (SyntaxTreeNode* child : this->children)
            {
                delete child;
            }
        }
        

        bool SyntaxTreeNode::is_expression() const
        {
            return this->type == SyntaxTreeNodeType::Comparison ||
                this->type == SyntaxTreeNodeType::Addition ||
                this->type == SyntaxTreeNodeType::Multiplication ||
                this->type == SyntaxTreeNodeType::Unary ||
                this->type == SyntaxTreeNodeType::Primary ||
                this->type == SyntaxTreeNodeType::Call;
        }

        bool SyntaxTreeNode::__flod_number_literals()
        {
            bool result_is_float = this->children[0]->token->type == LexicalAnalysis::TokenType::FloatLiteral || 
                (this->children.size() == 2 && this->children[1]->token->type == LexicalAnalysis::TokenType::FloatLiteral) ||
                this->token->type == LexicalAnalysis::TokenType::Op_Devide;
                
            if (!result_is_float)
            {
                long long floded_value = std::stoll(this->children[0]->token->value);
                switch (this->token->type)
                {
                case LexicalAnalysis::TokenType::Op_Plus:
                    floded_value += std::stoll(this->children[1]->token->value);
                    break;

                case LexicalAnalysis::TokenType::Op_Minus:
                    if (this->children.size() == 2)
                    {
                        floded_value -= std::stoll(this->children[1]->token->value);
                    }
                    else
                    {
                        floded_value = -floded_value;
                    }
                    break;

                case LexicalAnalysis::TokenType::Op_Multiply:
                    floded_value *= std::stoll(this->children[1]->token->value);
                    break;
                
                default:
                    // TODO
                    return false;
                }

                this->type = SyntaxTreeNodeType::Primary;
                this->token->type = LexicalAnalysis::TokenType::IntLiteral;
                this->token->value = std::to_string(floded_value);
                return true;
            }
            else  // result_is_float
            {
                double floded_value = std::stod(this->children[0]->token->value);
                switch (this->token->type)
                {
                case LexicalAnalysis::TokenType::Op_Plus:
                    floded_value += std::stod(this->children[1]->token->value);
                    break;

                case LexicalAnalysis::TokenType::Op_Minus:
                    if (this->children.size() == 2)
                    {
                        floded_value -= std::stod(this->children[1]->token->value);
                    }
                    else
                    {
                        floded_value = -floded_value;
                    }
                    break;

                case LexicalAnalysis::TokenType::Op_Multiply:
                    floded_value *= std::stod(this->children[1]->token->value);
                    break;

                case LexicalAnalysis::TokenType::Op_Devide:
                    if (this->children[1]->token->value == "0")
                    {
                        return false;  // TODO
                    }
                    floded_value /= std::stod(this->children[1]->token->value);
                    break;
                
                default:
                    // TODO
                    return false;
                }

                this->type = SyntaxTreeNodeType::Primary;
                this->token->type = LexicalAnalysis::TokenType::FloatLiteral;

                // 此处使用std::to_string会有较大精度损失
                std::stringstream ss;
                ss.precision(20);
                ss << floded_value;
                this->token->value = ss.str();
                
                return true;
            }
        }

        bool SyntaxTreeNode::try_fold_literal()
        {
            if (this->type == SyntaxTreeNodeType::Primary)
            {
                return this->token->is_literal() ? true : false;
            }

            if (this->children[0]->try_fold_literal() && (this->children.size() != 2 || this->children[1]->try_fold_literal()))
            {
                LexicalAnalysis::TokenType token_type_1 = this->children[0]->token->type;
                LexicalAnalysis::TokenType token_type_2 = LexicalAnalysis::TokenType::Unknown;
                if (this->children.size() == 2)
                {
                    token_type_2 = this->children[1]->token->type;
                }

                bool both_are_number = 
                    (token_type_1 == LexicalAnalysis::TokenType::IntLiteral || token_type_1 == LexicalAnalysis::TokenType::FloatLiteral) &&
                    (token_type_2 == LexicalAnalysis::TokenType::IntLiteral || token_type_2 == LexicalAnalysis::TokenType::FloatLiteral || 
                    token_type_2 == LexicalAnalysis::TokenType::Unknown);

                if (both_are_number)
                {
                    if (!this->__flod_number_literals())
                    {
                        return false;
                    }
                }
                else  // both_are_number == false
                {
                    return false;
                }
                
                delete this->children[0];
                if (this->children.size() == 2)
                {
                    delete this->children[1];
                }
                this->children.clear();
                return true;
            }
            return false;
        }

        #ifdef Debug_Write_AST_To_Json_File
        YJson::Object* SyntaxTreeNode::to_json() const
        {
            YJson::Dict dict;
            dict["type"] = new YJson::Object(to_string(this->type));
            if (this->token != nullptr)
            {
                YJson::String str = "";
                for (char chr : this->token->to_string())
                {
                    if (chr == '"')
                    {
                        str += "\\\"";
                    }
                    else
                    {
                        str.push_back(chr);
                    }
                }
                dict["value"] = new YJson::Object(str);
            }
            if (!this->children.empty())
            {
                YJson::List children_jsons;
                for (auto child : this->children)
                {
                    children_jsons.push_back(child->to_json());
                }
                dict["children"] = new YJson::Object(children_jsons);
            }
            return new YJson::Object(dict);
        }
        #endif
    }
}