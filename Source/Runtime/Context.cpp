#include <Runtime/Context.h>
#include <Runtime/MemoryManager.hpp>
#include <Exception/CoreException.h>


namespace XyA
{
    namespace Runtime
    {
        Context::Context(CodeObject* code_obj)
        {
            this->code_obj = code_obj;

            this->local_variables = XyA_Allocate_Array_(Object*, this->code_obj->variable_names.size());  // delete于Context::~Context()
            for (size_t i = 0; i < this->code_obj->variable_names.size(); i ++)
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

            /* 离开作用域, 所有变量引用计数减一 */
            for (size_t i = 0; i < this->code_obj->variable_names.size(); i ++)
            {
                if (this->local_variables[i] != nullptr)
                {
                    // printf("Dereference Variable:%s %s %zd\n", this->get_variable_name_at(i).c_str(), this->local_variables[i]->type()->name.c_str(), this->local_variables[i]->ref_count);
                    this->local_variables[i]->dereference();
                    // printf("END\n");
                }
            }

            XyA_Deallocate_Array(this->local_variables, this->code_obj->variable_names.size());
        }

        Instruction* Context::cur_instruction() const
        {
            return this->code_obj->instructions[this->instruction_ptr];
        }

        const Type* Context::cls() const
        {
            return this->code_obj->cls;
        }

        void Context::set_exception(StringView exception_message)
        {
            this->thrown_exception = XyA_Allocate(Builtin::BuiltinException, exception_message);
        }

        void Context::set_exception(BaseException* exception)
        {
            this->thrown_exception = exception;
        }

        Object* Context::get_literal_obj_at(size_t index) const
        {
            return this->code_obj->literals[index];
        }

        Object* Context::get_variable_at(size_t index) const
        {
            return this->local_variables[index];
        }

        void Context::set_variable_at(Id variable_id, Object* new_object)
        {
            Object* old_object = this->local_variables[variable_id];
            if (old_object != nullptr)
            {
                old_object->dereference();
            }

            this->local_variables[variable_id] = new_object;
            new_object->reference();
        }

        StringView Context::get_variable_name_at(size_t index) const
        {
            return this->code_obj->variable_names[index];
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