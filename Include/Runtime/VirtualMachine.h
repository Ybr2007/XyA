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

            Context* globle_context = nullptr;
            Context* cur_context = nullptr;

            static VirtualMachine& get_instance();
            void execute(Context* globle_context);
            void execute_context();

        private:
            VirtualMachine();

            void __init_global_context();
            void __excute_instruction(Instruction* instruction);
            void __back_context();

            void __call_compare_magic_method(const std::string& method_name);

            BaseFunction* __get_obj_method(Object* object, const std::string& method_name) const;

            void __throw_exception(std::string_view message) const;
        };
    }
}