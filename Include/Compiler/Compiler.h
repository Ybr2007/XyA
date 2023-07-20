#pragma once
#include <functional>
#include <LexicalAnalysis/Token.h>
#include <SyntaxAnalysis/SyntaxTree.h>
#include <Runtime/CodeObject.h>


namespace XyA
{
    namespace Compiler
    {
        using CompilerErrorCallback = std::function<void(std::string_view, LexicalAnalysis::TokenPos)>;

        class Compiler
        {
        public:
            Runtime::CodeObject* compile(SyntaxAnalysis::SyntaxTreeNode* syntax_tree_root);
            void register_error_callback(CompilerErrorCallback callback);

        private:
            /*
            在以下__compile_xxx中new的Instructions都会在Runtime::CodeObject::~CodeObject释放
            在以下__compile_xxx中new的Literals会被GC机制释放
             */
            Runtime::CodeObject* __global_code_object;
            std::vector<CompilerErrorCallback> __error_callbacks;

            void __init_builtins();

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
            Runtime::Attr __build_method(SyntaxAnalysis::SyntaxTreeNode* method_definition_root, Runtime::Type* cls);
            Runtime::Type* __build_class(
                Runtime::CodeObject* code_object, SyntaxAnalysis::SyntaxTreeNode* class_definition_root, size_t& class_variable_index);

            void __throw_error(std::string_view message, LexicalAnalysis::TokenPos pos) const;
        };

    }  // namespace Compiler
}  // namespace XyA