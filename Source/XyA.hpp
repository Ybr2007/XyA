#pragma once
#include <iostream>
#include <string>
#include <format>
#include <LexicalAnalysis/TokenAnalyzer.h>
#include <SyntaxAnalysis/SyntaxParser.h>
#include <Compiler/Compiler.h>
#include <Runtime/VirtualMachine.h>
#include <Config.h>
#include <Runtime/MemoryManager.hpp>
#include <Runtime/CustomFunction.h>


namespace XyA
{
    const std::string error_color = "\033[1;31m";
    const std::string reset_color = "\033[0m";

    class Core
    {
    public:
        std::string_view excuting_source;
        LexicalAnalysis::TokenAnalyzer token_analyzer;
        SyntaxAnalysis::SyntaxParser syntax_parser;
        Compiler::Compiler compiler;
        Runtime::VirtualMachine* virtual_machine = Runtime::VirtualMachine::get_instance();

        std::vector<std::pair<std::string, LexicalAnalysis::TokenPos>> error_messages;
        bool exception_thrown;

        Core();
        void execute(std::string_view source);
        void print_messages();
        std::string mark_pos(LexicalAnalysis::TokenPos pos);
    };

    Core::Core()
    {
        this->token_analyzer.exception_callbacks.push_back(
            [&](std::string_view msg, LexicalAnalysis::TokenPos pos){
                this->error_messages.push_back(
                    std::make_pair(std::format("{}Lexical Error: {} {}  Pos: row {}, column {}\n",
                        error_color, reset_color, std::string(msg).c_str(), pos.row, pos.column), pos)
                );
                this->exception_thrown = true;
            }
        );

        this->syntax_parser.exception_callbacks.push_back(
            [&](std::string_view msg, LexicalAnalysis::TokenPos pos){
                this->error_messages.push_back(
                    std::make_pair(std::format("{}Syntax Error: {} {}  Pos: row {}, column {}\n",
                        error_color, reset_color, std::string(msg).c_str(), pos.row, pos.column), pos)
                );
                this->exception_thrown = true;
            }
        );

        this->compiler.register_error_callback(
            [&](std::string_view msg, LexicalAnalysis::TokenPos pos){
                this->error_messages.push_back(
                    std::make_pair(std::format("{}Compiling Error: {} {}  Pos: row {}, column {}\n",
                        error_color, reset_color, std::string(msg).c_str(), pos.row, pos.column), pos)
                );
                this->exception_thrown = true;
            }
        );

        this->virtual_machine->exception_callbacks.push_back(
            [&](std::string_view msg){
                printf("%sRuntime Error%s: %s\n", error_color.c_str(), reset_color.c_str(), std::string(msg).c_str());
            }
        );
    }

    void Core::execute(std::string_view source)
    {
        if (source.empty())
        {
            return;
        }

        this->exception_thrown = false;
        this->excuting_source = source;

        // 词法分析, 生成Tokens
        std::vector<LexicalAnalysis::Token*>* tokens = this->token_analyzer.analyze_source(source);

        if (this->exception_thrown)
        {
            this->print_messages();
            return;
        }

        #ifdef Debug_Display_Tokens
        printf("Tokens:\n");
        for (auto token : *tokens)
        {
            printf("%s ", token->to_string().c_str());
        }
        printf("\n\n");
        #endif

        // 语法分析, 生成语法树
        SyntaxAnalysis::SyntaxTreeNode* syntax_tree = this->syntax_parser.parse_tokens(tokens);

        if (this->exception_thrown)
        {
            this->print_messages();
            return;
        }

        #ifdef Debug_Write_AST_To_Json_File
        YJson::Object* syntax_tree_json = syntax_tree->to_json();
        YJson::dump("Y:\\C++\\XyA\\syntax_tree.json", syntax_tree_json, 2);
        delete syntax_tree_json;
        #endif

        // 编译, 生成CodeObject
        Runtime::CodeObject* code_object = this->compiler.compile(syntax_tree);

        if (this->exception_thrown)
        {
            this->print_messages();
            return;
        }

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
            Runtime::CustomFunction* function = dynamic_cast<Runtime::CustomFunction*>(iter.second);
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

                    Runtime::CustomFunction* method = dynamic_cast<Runtime::CustomFunction*>(iter->value.object);

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

        // 编译结束, Tokens、SyntaxTree不再使用, 释放
        for (LexicalAnalysis::Token* token : *tokens)
        {
            delete token;
        }
        delete tokens;
        delete syntax_tree;

        // 启动虚拟机
        this->virtual_machine->execute(code_object);

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

    void Core::print_messages()
    {
        std::cout << std::format("{}{} Errors!{}\n", error_color, this->error_messages.size(), reset_color);
        for (const auto& message : this->error_messages)
        {
            std::cout << message.first;
            std::cout << this->mark_pos(message.second) << std::endl;
        }
    }

    std::string Core::mark_pos(LexicalAnalysis::TokenPos pos)
    {
        size_t start_pos = 0;
        size_t end_pos = this->excuting_source.find('\n');

        for (size_t i = 1; i < pos.row; i++)
        {
            if (end_pos == std::string_view::npos)
            {
                // 已经到达最后一行
                return "";
            }
            start_pos = end_pos + 1;
            end_pos = this->excuting_source.find('\n', start_pos);
        }

        std::string row_str = std::to_string(pos.row);
        std::string line;
        if (end_pos == std::string_view::npos)
        {
            line = std::string(this->excuting_source.substr(start_pos, this->excuting_source.length() - start_pos));
        }
        else
        {
            line = std::string(this->excuting_source.substr(start_pos, end_pos - start_pos));
        }

        line += "\n";
        for (size_t i = 1; i < pos.column + row_str.length() + 3; i ++)
        {
            line += ' ';
        }
        line += "^";
        return std::format("{} | {}", row_str, line);
    }
}