#pragma once
#include <functional>
#include <LexicalAnalysis/Token.h>
#include <SyntaxAnalysis/SyntaxTree.h>


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
            ! 警告：该方法有越界风险, 建议调用前使用if(!this->__at_end())进行检查
            */
            LexicalAnalysis::Token* __next_token() const;
            LexicalAnalysis::Token* __prev_token() const;
            bool __try_move_ptr(size_t step=1);
            void __backtrack(size_t step=1);
            bool __at_end() const;

            /*
            以下"__parse_"开头的方法对应着每一个语法元素
            在Source/SyntaxAnalysis/Grammar文件查看语法定义
            当这些方法被调用时, __cur_pos应指向要解析的语法元素的开头Token。当方法结束时, __cur_pos应指向语法元素的最后一个Token。
            默认第一个token已经被检验
            */

            SyntaxTreeNode* __parse_unit();
            SyntaxTreeNode* __parse_line();
            SyntaxTreeNode* __parse_block();
            SyntaxTreeNode* __parse_if();
            SyntaxTreeNode* __parse_while();
            SyntaxTreeNode* __parse_type_hint();
            SyntaxTreeNode* __parse_assignment(SyntaxTreeNode* assignment_target, SyntaxTreeNode* type_hint=nullptr);
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
    }  // namespace SyntaxAnalysis
}  // namespace XyA