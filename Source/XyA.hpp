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
            [](std::string_view msg, LexicalAnalysis::TokenPos pos){
                printf("Lexical Error: %s  Pos: %zu %zu\n", std::string(msg).c_str(), pos.row, pos.column);
            }
        );

        this->syntax_parser.exception_callbacks.push_back(
            [](std::string_view msg, LexicalAnalysis::TokenPos pos){
                printf("Syntax Error: %s  Pos: %zu %zu\n", std::string(msg).c_str(), pos.row, pos.column);
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

        #ifdef Debug_Display_Tokens
        printf("Tokens:\n");
        for (auto token : *tokens)
        {
            printf("%s ", token->to_string().c_str());
        }
        printf("\n\n");
        #endif

        // 语法分析，生成语法树
        SyntaxAnalysis::SyntaxTreeNode* syntax_tree = this->syntax_parser.parse_tokens(tokens);

        #ifdef Debug_Write_AST_To_Json_File
        YJson::Object* syntax_tree_json = syntax_tree->to_json();
        YJson::dump("Y:\\C++\\XyA\\syntax_tree.json", syntax_tree_json, 2);
        delete syntax_tree_json;
        #endif

        // 编译，生成CodeObject
        Runtime::CodeObject* code_object = this->compiler.compile(syntax_tree);

        #ifdef Debug_Write_AST_To_Json_File
        YJson::Object* optimized_syntax_tree_json = syntax_tree->to_json();
        YJson::dump("Y:\\C++\\XyA\\syntax_tree_optimized.json", optimized_syntax_tree_json, 2);
        delete optimized_syntax_tree_json;
        #endif

        #ifdef Debug_Display_Instructions
        printf("Global Instructions:\n");
        for (size_t i = 0; i < code_object->instructions.size(); i ++)
        {
            printf("%d %s\n", (int)i, code_object->instructions[i]->to_string().c_str());
        }
        printf("\n");
        
        printf("Function Instructions:\n");
        for (auto iter : code_object->prebuilt_objects)
        {
            printf("-----------------------\n");
            Runtime::Function* function = dynamic_cast<Runtime::Function*>(iter.second);
            if (function)
            {
                for (size_t i = 0; i < function->code_object->instructions.size(); i ++)
                {
                    printf("%d %s\n", (int)i, function->code_object->instructions[i]->to_string().c_str());
                }
            }
            else
            {
                for (const auto& iter : dynamic_cast<Runtime::Type*>(iter.second)->attrs)
                {
                    printf("Method: %s\n", iter->key.c_str());

                    Runtime::Function* method = dynamic_cast<Runtime::Function*>(iter->value.object);

                    for (size_t i = 0; method && i < method->code_object->instructions.size(); i ++)
                    {
                        printf("%d %s\n", (int)i, method->code_object->instructions[i]->to_string().c_str());
                    }
                }
            }
            
            printf("\n");
        }
        printf("\n");
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