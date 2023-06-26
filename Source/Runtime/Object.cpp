#pragma once
#include <Runtime/Object.h>


namespace XyA
{
    namespace Runtime
    {
        Object::~Object()
        {
            // printf("Delete %s\n", this->type == nullptr ? dynamic_cast<Type*>(this)->name.c_str() : this->type->name.c_str());
        }

        bool Object::dwindle_ref_count()
        {
            if (-- this->ref_count <= 0)
            {
                #ifndef DebugMode
                delete this;
                #endif
                return true;
            }
            return false;
        }
        
        TryGetMethodResult Object::try_get_method(const std::string& method_name, BaseFunction*& result) const
        {
            auto iter = this->attrs.find(method_name);
            if (iter == this->attrs.end())
            {
                iter = this->type->attrs.find(method_name);
                if (iter == this->type->attrs.end())
                {
                    return TryGetMethodResult::NotFound;
                }
            }
            BaseFunction* result_buffer = dynamic_cast<BaseFunction*>(iter->second);
            if (result_buffer == nullptr)
            {
                return TryGetMethodResult::NotCallable;
            }
            result = result_buffer;
            return TryGetMethodResult::OK;
        }
    }
}