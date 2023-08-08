#include <Runtime/Module.h>


namespace XyA
{
    namespace Runtime
    {
        ModuleType* ModuleType::get_instance()
        {
            static ModuleType instance;
            return &instance;
        }

        ModuleType::ModuleType()  // private
        {
            Type("module");
        }

        Module::Module(CodeObject* code_object)
        {
            this->__type = Module::static_type();
        }

        ModuleType* Module::static_type()
        {
            return ModuleType::get_instance();
        }

    }
}