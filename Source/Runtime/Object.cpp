#pragma once
#include <Runtime/Object.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        Object::~Object()
        {
            for (const auto& iter : this->attrs)
            {
                iter.second->dereference();
            }
        }

        void Object::reference()
        {
            this->ref_count ++;
        }

        bool Object::dereference()
        {
            if (this->ref_count == 0)
            {
                printf("ERROR: ref_count < 0\n");
                #ifdef Debug_Display_Object
                printf("Object: %s\n", this->to_string().c_str());
                #endif
                exit(-1);
            }
            
            if (-- this->ref_count == 0)
            {
                XyA_Deallocate(this);
                return true;
            }
            return false;
        }

        bool Object::deallocate_if_no_ref()
        {
            if (this->ref_count < 0)
            {
                printf("ERROR: ref_count < 0\n");
                #ifdef Debug_Display_Object
                printf("Object: %s\n", this->to_string().c_str());
                #endif
                exit(-1);
            }

            if (this->ref_count == 0)
            {
                XyA_Deallocate(this);
                return true;
            }

            return false;
        }

        bool Object::is_instance(Type* type) const
        {
            return this->type == type;
        }
        
        TryGetMethodResult Object::try_get_method(const std::string& method_name, BaseFunction*& result) const
        {
            auto iter = this->attrs.find(method_name);
            if (iter == this->attrs.end())
            {
                if (this->type == nullptr)
                {
                    return TryGetMethodResult::NotFound;
                }

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

        #ifdef Debug_Display_Object
        std::string Object::to_string() const
        {
            return this->type == nullptr ? 
                "<XyA Object at " + std::to_string((size_t)this) + ">" : 
                "<" + this->type->name + " Object at " + std::to_string((size_t)this) + ">" ;
        }
        #endif
    }
}