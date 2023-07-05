#pragma once
#include <functional>
#include <Runtime/Context.h>

#include <Runtime/Instruction.h>
#include <Runtime/Object.h>
#include <Runtime/MagicMethodNames.hpp>
#include <Runtime/Builtin/BuiltinFunctions.h>

#include <Config.h>


namespace XyA
{
    namespace Runtime
    {
        class VirtualMachine
        {
        public:
            std::vector<std::function<void(std::string_view)>> exception_callbacks;

            Context* global_context = nullptr;
            Context* cur_context = nullptr;

            static VirtualMachine* get_instance();
            void execute(Context* global_context);
            void execute_context();

        private:
            VirtualMachine();

            void __init_global_context();
            void __excute_instruction(Instruction* instruction);
            void __back_context();

            Object* __call_object(Object* callee, Object** args, size_t arg_num, bool& exception_thrown);
            void __call_binary_operation_magic_method(size_t magic_method_index);
            void __call_compare_magic_method(size_t magic_method_index);

            void __throw_exception(std::string_view message) const;
        };
    }
}