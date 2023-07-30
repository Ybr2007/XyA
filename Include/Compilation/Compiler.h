#pragma once
#include <optional>
#include <functional>
#include <LexicalAnalysis/Token.h>
#include <SyntaxAnalysis/SyntaxTree.h>
#include <Runtime/CodeObject.h>
#include <Utils/Message.h>


namespace XyA
{
    namespace Compilation
    {
        class Compiler
        {
        public:
            Runtime::CodeObject* compile(SyntaxAnalysis::SyntaxTreeNode* syntax_tree_root);
            void register_message_callback(MessageCallback callback);

        private:
            Runtime::CodeObject* __global_code_object;
            std::vector<MessageCallback> __message_callbacks;

            /* 编译过程中记录所处上下文的数据 */
            bool __inside_judgment_block = false;                       // 是否在判断语句块中
            bool __inside_loop_block = false;                           // 是否在循环语句中

            bool __inside_method = false;                               // 是否在方法中
            bool __inside_constructor = false;                          // 是否在构造函数中
            StringView __first_argument_name_of_method;                 // 方法的第一个参数名称（传入self的参数）
            std::unordered_set<Id> __assignments_inside_constructor;    // 构造函数中self被赋值的属性名Id

            void __init_builtins();

            /*
            在以下__compile_xxx中new的Instructions都会在Runtime::CodeObject::~CodeObject释放
            在以下__compile_xxx中new的Literals会被GC机制释放
             */
            void __compile_unit(Runtime::CodeObject* code_object, SyntaxAnalysis::SyntaxTreeNode* unit_root);
            void __compile_line(Runtime::CodeObject* code_object, SyntaxAnalysis::SyntaxTreeNode* line_root);
            void __compile_block(Runtime::CodeObject* code_object, SyntaxAnalysis::SyntaxTreeNode* block_root);
            void __compile_if(Runtime::CodeObject* code_object, SyntaxAnalysis::SyntaxTreeNode* while_root);
            void __compile_while(Runtime::CodeObject* code_object, SyntaxAnalysis::SyntaxTreeNode* if_root);
            void __compile_assignment(Runtime::CodeObject* code_object, SyntaxAnalysis::SyntaxTreeNode* assignment_root);
            void __compile_expression(Runtime::CodeObject* code_object, SyntaxAnalysis::SyntaxTreeNode* expression_root, bool pop=false);
            void __compile_return(Runtime::CodeObject* code_object, SyntaxAnalysis::SyntaxTreeNode* return_root);

            Runtime::CustomFunction* __build_function(
                Runtime::CodeObject* code_object, SyntaxAnalysis::SyntaxTreeNode* function_definition_root, size_t& function_variable_index);
            std::optional<Runtime::Attr> __build_method(SyntaxAnalysis::SyntaxTreeNode* method_definition_root, Runtime::Type* cls);
            Runtime::Type* __build_class(
                Runtime::CodeObject* code_object, SyntaxAnalysis::SyntaxTreeNode* class_definition_root, size_t& class_variable_index);

            void __throw_error(StringView message, LexicalAnalysis::TokenPos pos) const;
            void __send_message(Message message, LexicalAnalysis::TokenPos pos) const;
        };

    }  // namespace Compilation
}  // namespace XyA