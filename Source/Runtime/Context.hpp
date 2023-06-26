#pragma once
#include <stack>
#include <Runtime/Object.hpp>
#include <Runtime/CodeObject.hpp>


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

        Context::Context(CodeObject* code_obj)
        {
            this->code_obj = code_obj;

            this->local_variables = new Object*[code_obj->variable_name_indices.size()];
            for (size_t i = 0; i < code_obj->variable_name_indices.size(); i ++)
            {
                this->local_variables[i] = nullptr;
            }
        }

        Context::~Context()
        {
            if (!this->operand_stack.empty())
            {
                printf("WARNING: Stack is not empty when the context exits.");
            }

            for (size_t i = 0; i < this->code_obj->variable_name_indices.size(); i ++)
            {
                this->local_variables[i]->dwindle_ref_count();
            }
            delete this->local_variables;
            delete this->code_obj;
        }

        Instruction* Context::cur_instruction() const
        {
            return this->code_obj->instructions[this->instruction_ptr];
        }

        Object* Context::get_literal_obj_at(size_t index) const
        {
            return this->code_obj->literals[index];
        }

        Object* Context::get_variable_at(size_t index) const
        {
            return this->local_variables[index];
        }

        const std::string& Context::get_variable_name(size_t index) const
        {
            for (auto& item : this->code_obj->variable_name_indices)
            {
                if (item.second == index)
                {
                    return item.first;
                }
            }
        }

        void Context::push_operand(Object* obj)
        {
            this->operand_stack.push(obj);
        }

        Object* Context::pop_operand()
        {
            Object* top_obj = this->operand_stack.top();
            this->operand_stack.pop();
            return top_obj;
        }

        Object* Context::top_operand() const
        {
            return this->operand_stack.top();
        }
    }
}