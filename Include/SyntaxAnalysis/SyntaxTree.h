#pragma once
#include <vector>
#include <sstream>
#include <LexicalAnalysis/Token.h>
#include <Config.h>
#ifdef Debug_Write_AST_To_Json_File
#include <YJson/YJson.h>
#endif

namespace XyA
{
    namespace SyntaxAnalysis
    {
        enum class SyntaxTreeNodeType
        {
            // children: 数量不限, 每个child是一个语句（块）
            Root,
            // children: 数量不限, 每个child是一个语句（块）
            Block,
            // children: 数量为2或3, children[0] 是判断条件表达式, children[1] 是如果为真执行的语句块, children[2] 是如果为假执行的语句块（可选）
            If,
            // children: 数量为2, children[0] 是判断条件表达式, children[1] 是如果为真执行的语句块
            While,
            // children: 数量为2或3, chilren[0]是赋值目标, children[1] 是值的表达式, children[2]是类型标注(可选)
            Assignment,
            // token: 运算符
            // chilren: 数量为2, 为两个运算数
            Comparison,
            // token: 运算符
            // chilren: 数量为2, 为两个运算数
            Addition,
            // token: 运算符
            // chilren: 数量为2, 为两个运算数
            Multiplication,
            // token: 运算符
            // chilren: 数量为1, 为运算数
            Unary,
            // token: 标识符或字面量
            Primary,
            // token: 函数名称标识符
            // children: 数量为2或3, children[0] 形参列表(Argument_List), children[1] 函数体(Block)，children[2] 返回值类型标注
            Function_Definition,
            // children: 数量不限, 每一个child是一个参数（对于Definition_Argument_List, 是形参标识符；对于Call_Argument_List, 是实参表达式）
            Argument_List,
            // token: 形参名称标识符
            // children: 数量为0或1，children[0] 为形参的类型标注(可选)
            Argument_Definition,
            // chilren: 数量为2, children[0] 为callee表达式, children[1] 为实参列表(Argument_List)
            Call,
            // token: 属性标识符
            // children: 数量为1或2, chilren[0] 为属性的所有者, children[1] 为修饰符(Modifier, 仅当node作为类内attr赋值对象时可选)
            Attr,
            // children: 数量为1, chilren[0] 为返回值表达式
            Return,
            // token: 修饰符
            Modifier,
            // token: 方法名称标识符
            // children: 数量为2或3, children[0] 形参列表(Argument_List), children[1] 函数体(Block), children[2] 方法可访问性修饰符（可选）
            Method_Definition,
            // token: 类名称标识符
            // chilren: 数量不限, 每一个child是一个成员函数定义(Function_Definition)
            Class_Definition,
            // token: 类型标识符(Identifier)
            Type,
            // children: 数量不限,每一个child是一个可能的类型(Type)
            Union_Type_Hint,
            // children: 数量不限,每一个child是一个除null外可能的类型(Type)
            Optional_Type_Hint,
            // children: 数量为2, children[0] 为对象表达式, children[1] 为目标类型标识符(Identifier Primary)
            Type_Conversion,
            // token: 配置值字面量(Literal)
            Configuration_Value,
            // token: 被配置项名称(Identifier)
            // children: 数量为1, children[0] 为配置值(Configuration_Value)
            Configuration_Command,
        };

        #ifdef Debug_Write_AST_To_Json_File
        std::string to_string(SyntaxTreeNodeType type);
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
            ! 警告：不会检查node类型, 调用前确定is_expression()为true
            */
            bool try_fold_literal();

            #ifdef Debug_Write_AST_To_Json_File
            YJson::Object* to_json() const;
            #endif

        private:
            bool __flod_number_literals();
        };
        
    }
}