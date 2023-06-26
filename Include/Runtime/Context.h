#pragma once
#include <stack>
#include <Runtime/Object.h>
#include <Runtime/CodeObject.h>


namespace XyA
{
    namespace Runtime
    {
        class Context
        {
        public:
            Context* back = nullptr;
            CodeObject* code_obj;
            Object** local_variables;
            std::stack<Object*> operand_stack;
            size_t instruction_ptr = 0;

            Object* thrown_obj = nullptr;

            Context(CodeObject* code_obj);
            ~Context();

            Instruction* cur_instruction() const;

            Object* get_literal_obj_at(size_t index) const;
            Object* get_variable_at(size_t index) const;
            const std::string& get_variable_name(size_t index) const;

            void push_operand(Object* obj);
            Object* pop_operand();
            Object* top_operand() const;
        };
    }
}