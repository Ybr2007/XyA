#pragma once
#include <functional>
#include <Runtime/BasicObjectDefs.h>
#include <Runtime/Context.h>
#include <Runtime/Instruction.h>
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
            void execute(CodeObject* global_code_object);
            void execute_context();

        private:
            VirtualMachine() = default;
            VirtualMachine(const VirtualMachine& other) = delete;

            void __init_global_context();
            void __excute_instruction(Instruction* instruction);
            void __back_context();

            void __call_binary_operation_magic_method(const std::string& magic_method_name);
            void __call_compare_magic_method(const std::string& magic_method_name);

            void __throw_exception();
        };
    }
}