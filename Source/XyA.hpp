#pragma once
#include <iostream>
#include <string>
#include <LexicalAnalysis/TokenAnalyzer.hpp>
#include <SyntaxAnalysis/SyntaxParser.hpp>
#include <Compiler/Compiler.hpp>
#include <Runtime/VirtualMachine.h>
#include <Config.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    class Core
    {
    public:
        LexicalAnalysis::TokenAnalyzer token_analyzer;
        SyntaxAnalysis::SyntaxParser syntax_parser;
        Compiler::Compiler compiler;
        Runtime::VirtualMachine* virtual_machine = Runtime::VirtualMachine::get_instance();

        Core();
        void execute(std::string_view source);
    };

    Core::Core()
    {
        this->token_analyzer.exception_callbacks.push_back(
            [](std::string_view msg, size_t pos){
                printf("Lexical Error: %s  Pos: %zu\n", std::string(msg).c_str(), pos);
                exit(-1);
            }
        );

        this->syntax_parser.exception_callbacks.push_back(
            [](std::string_view msg, size_t pos){
                printf("Syntax Error: %s  Pos: %zu\n", std::string(msg).c_str(), pos);
                exit(-1);
            }
        );

        this->virtual_machine->exception_callbacks.push_back(
            [](std::string_view msg){
                printf("Runtime Error: %s\n", std::string(msg).c_str());
                exit(-1);
            }
        );
    }

    void Core::execute(std::string_view source)
    {
        if (source.empty())
        {
            return;
        }

        // 词法分析，生成Tokens
        std::vector<LexicalAnalysis::Token*>* tokens = this->token_analyzer.analyze_source(source);

        // #ifdef DebugMode
        // printf("Tokens:\n");
        // for (auto token : *tokens)
        // {
        //     printf("%s ", token->to_string().c_str());
        // }
        // printf("\n\n");
        // #endif

        // 语法分析，生成语法树
        SyntaxAnalysis::SyntaxTreeNode* syntax_tree = this->syntax_parser.parse_tokens(tokens);

        // #ifdef DebugMode
        // YJson::Object* syntax_tree_json = syntax_tree->to_json();
        // YJson::dump("Y:\\C++\\XyA\\syntax_tree.json", syntax_tree_json, 2);
        // delete syntax_tree_json;
        // #endif

        // 编译，生成CodeObject
        Runtime::CodeObject* code_object = this->compiler.compile(syntax_tree);

        #ifdef Debug_Display_Instructions
        printf("Global Instructions:\n");
        for (size_t i = 0; i < code_object->instructions.size(); i ++)
        {
            printf("%d %s\n", (int)i, code_object->instructions[i]->to_string().c_str());
        }
        printf("\n");
        
        printf("Function Instructions:\n");
        for (const auto& iter : code_object->functions)
        {
            printf("Function Name: %s\n", iter.first.c_str());
            for (size_t i = 0; i < iter.second->code_object->instructions.size(); i ++)
            {
                printf("%d %s\n", (int)i, iter.second->code_object->instructions[i]->to_string().c_str());
            }
            printf("\n");
        }
        printf("\n");
        #endif

        // printf("Literals:\n");
        // for (size_t i = 0; i < code_object->literals.size(); i ++)
        // {
        //     Runtime::Builtin::StringObject* str_obj = dynamic_cast<Runtime::Builtin::StringObject*>(code_object->literals[i]);
        //     if (str_obj == nullptr)
        //     {
        //         Runtime::BaseFunction* str_method = dynamic_cast<Runtime::BaseFunction*>(
        //             code_object->literals[i]->type->attrs[Runtime::MagicMethodNames::str_method_name]);

        //         Runtime::Object** args = new Runtime::Object*[1]{code_object->literals[i]};
        //         bool exception_thrown = false;
        //         str_obj = dynamic_cast<Runtime::Builtin::StringObject*>(str_method->call(args, 1, exception_thrown));
        //     }
        //     printf("index: %d  value: %s  ref_count: %d\n", (int)i, str_obj->value.c_str(), code_object->literals[i]->ref_count);
        // }
        // printf("\n");
        // #endif

        #ifdef Debug_Write_AST_To_Json_File
        YJson::Object* optimized_syntax_tree_json = syntax_tree->to_json();
        YJson::dump("Y:\\C++\\XyA\\syntax_tree_optimized.json", optimized_syntax_tree_json, 2);
        delete optimized_syntax_tree_json;
        #endif

        // 编译结束，Tokens、SyntaxTree不再使用，释放
        for (LexicalAnalysis::Token* token : *tokens)
        {
            delete token;
        }
        delete tokens;
        delete syntax_tree;

        // 构建全局Context
        Runtime::Context* global_context = new Runtime::Context(code_object);

        // 启动虚拟机
        this->virtual_machine->execute(global_context);

        #ifdef DebugMode
        printf("\nVariables:\n");
        for (auto item : global_context->code_obj->variable_name_indices)
        {
            if (global_context->local_variables[item.second] == nullptr)
            {
                printf("<NULL>\n");
                continue;
            }
            Runtime::Builtin::StringObject* str_obj = dynamic_cast<Runtime::Builtin::StringObject*>(global_context->local_variables[item.second]);
            if (str_obj == nullptr)
            {
                Runtime::BaseFunction* str_method = dynamic_cast<Runtime::BaseFunction*>(
                    global_context->local_variables[item.second]->type->attrs[Runtime::MagicMethodNames::str_method_name]);
                Runtime::Object** args = new Runtime::Object*[1]{global_context->local_variables[item.second]};
                bool exception_thrown = false;
                str_obj = dynamic_cast<Runtime::Builtin::StringObject*>(str_method->call(args, 1, exception_thrown));
            }

            printf("index: %d  name: %s  value: %s  ref_count: %d\n", 
                item.second, item.first.c_str(), str_obj->value.c_str(), global_context->local_variables[item.second]->ref_count);
        }
        printf("\n");
        #endif
    }
}