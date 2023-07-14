#pragma once
#include <stack>
#include <Utils/Stack.hpp>
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
            Utils::Stack<Object*> operand_stack;
            size_t instruction_ptr = 0;

            Object* returned_obj = nullptr;
            BaseException* thrown_exception = nullptr;

            Context(CodeObject* code_obj);
            ~Context();

            Instruction* cur_instruction() const;

            /*
            获取该Context的CodeObject所代表的function是哪个类的method
            如果不是method则返回nullptr
            */
            const Type* cls() const;

            void set_exception(std::string_view exception_message);
            void set_exception(BaseException* exception);

            Object* get_literal_obj_at(size_t index) const;
            Object* get_variable_at(size_t index) const;
            void set_variable_at(size_t index, Object* new_object);

            const std::string& get_variable_name_at(size_t index) const;

            void push_operand(Object* obj);
            Object* pop_operand();
            Object* top_operand() const;
            void set_top_operand(Object* obj);
        };
    }
}