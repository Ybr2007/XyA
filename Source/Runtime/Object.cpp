#pragma once
#include <Runtime/Object.h>
#include <Runtime/Builtin/BuiltinFunction.h>
#include <Runtime/Function.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        Object::~Object()
        {
            this->dereference_attrs();
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

        void Object::reference_attrs()
        {
            for (auto iter : this->attrs)
            {
                iter.second->reference();
            }
            for (auto magic_method : this->magic_methods)
            {
                if (magic_method != nullptr)
                {
                    magic_method->reference();
                }
            }
        }

        void Object::dereference_attrs()
        {
            for (auto iter : this->attrs)
            {
                iter.second->dereference();
            }
            for (auto magic_method : this->magic_methods)
            {
                if (magic_method != nullptr)
                {
                    magic_method->dereference();
                }
            }
        }

        bool Object::is_instance(Type* type) const
        {
            return this->type == type;
        }

        TryGetMethodResult Object::try_get_magic_method(size_t magic_method_index, BaseFunction*& result) const
        {
            Object* magic_method = this->magic_methods[magic_method_index];
            if (magic_method == nullptr)
            {
                if (this->type == nullptr)
                {
                    return TryGetMethodResult::NotFound;
                }

                magic_method = this->type->instance_magic_methods[magic_method_index];
                if (magic_method == nullptr)
                {
                    return TryGetMethodResult::NotFound;
                }
            }
            result = dynamic_cast<BaseFunction*>(magic_method);
            if (result != nullptr)
            {
                return TryGetMethodResult::OK;
            }
            return TryGetMethodResult::NotCallable;
        }

        TryGetAttrResult Object::try_get_attr(const std::string& attr_name, Object*& result) const
        {
            auto iter = this->attrs.find(attr_name);
            if (iter == this->attrs.end())
            {
                return TryGetAttrResult::NotFound;
            }
            result = iter->second;
            return TryGetAttrResult::OK;
        }
        
        #ifdef Debug_Display_Object
        std::string Object::to_string() const
        {
            return this->type == nullptr ? 
                "<XyA Object at " + std::to_string((size_t)this) + ">" : 
                "<" + this->type->name + " Object at " + std::to_string((size_t)this) + ">" ;
        }
        #endif

        Type::~Type()
        {            
            for (auto iter : this->instance_methods)
            {
                iter.second->dereference();
            }
            for (auto magic_method : this->instance_magic_methods)
            {
                if (magic_method != nullptr)
                {
                    magic_method->dereference();
                }
            }
        }

        void Type::reference_attrs()
        {
            Object::reference_attrs();

            for (auto iter : this->instance_methods)
            {
                iter.second->reference();
            }
            for (auto magic_method : this->instance_magic_methods)
            {
                if (magic_method != nullptr)
                {
                    magic_method->reference();
                }
            }
        }

    }
}