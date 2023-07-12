#pragma once
#include <Runtime/Context.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        Context::Context(CodeObject* code_obj)
        {
            this->code_obj = code_obj;

            this->local_variables = new Object*[code_obj->variable_name_2_index.size()];  // delete于Context::~Context()
            for (size_t i = 0; i < code_obj->variable_name_2_index.size(); i ++)
            {
                this->local_variables[i] = nullptr;
            }

            for (auto iter : this->code_obj->prebuilt_objects)
            {
                this->local_variables[iter.first] = iter.second;
            }
        }

        Context::~Context()
        {
            if (!this->operand_stack.empty())
            {
                printf("WARNING: The stack was not empty when the context exits.");
            }

            /* 离开作用域，所有变量引用计数减一 */
            for (size_t i = 0; i < this->code_obj->variable_name_2_index.size(); i ++)
            {
                if (this->local_variables[i] != nullptr)
                {
                    // printf("Dereference Variable:%s %s %zd\n", this->get_variable_name(i).c_str(), this->local_variables[i]->type()->name.c_str(), this->local_variables[i]->ref_count);
                    this->local_variables[i]->dereference();
                    // printf("END\n");
                }
            }

            delete this->local_variables;
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
            for (auto& item : this->code_obj->variable_name_2_index)
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
            if (this->operand_stack.empty())
            {
                printf("EMPTY Stack");
                exit(-1);
            }
            Object* top_obj = this->operand_stack.pop();
            return top_obj;
        }

        Object* Context::top_operand() const
        {
            if (this->operand_stack.empty())
            {
                printf("EMPTY Stack");
                exit(-1);
            }
            return this->operand_stack.top();
        }

        void Context::set_top_operand(Object* obj)
        {
            this->operand_stack.set_top(obj);
        }
    }
}