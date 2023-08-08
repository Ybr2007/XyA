#pragma once
#include <Runtime/BasicObjectDefs.h>
#include <Runtime/CodeObject.h>


namespace XyA
{
    namespace Runtime
    {

        class ModuleType : public Type
        {
        public:
            static ModuleType* get_instance();

        private:
            ModuleType();
            ModuleType(const ModuleType&) = delete;
            ModuleType(ModuleType&&) = delete;
            ModuleType&& operator=(const ModuleType&) = delete;
        };

        class Module : public Object
        {
        public:
            Module(CodeObject* code_object);
            
            static ModuleType* static_type();
        };

    }
}