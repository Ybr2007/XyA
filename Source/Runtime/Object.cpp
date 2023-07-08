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

        Type* Object::type() const
        {
            return this->__type != nullptr ? this->__type : Type::get_instance();
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
            return this->type() == type;
        }

        TryGetAttrResult Object::try_get_attr(const std::string& attr_name, Object*& result) const
        {
            if (this->attrs.try_get(attr_name, result))
            {
                return TryGetAttrResult::OK;
            }
            return TryGetAttrResult::NotFound;
        }

        TryGetMethodResult Object::try_get_magic_method(size_t index, BaseFunction*& result) const
        {
            Object* magic_method = this->magic_methods[index];
            if (magic_method == nullptr)
            {
                magic_method = this->type()->magic_methods[index];
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

        #ifdef Debug_Display_Object
        std::string Object::to_string() const
        {
            return this->type == nullptr ? 
                "<XyA Object at " + std::to_string((size_t)this) + ">" : 
                "<" + this->type->name + " Object at " + std::to_string((size_t)this) + ">" ;
        }
        #endif

        Type* Type::get_instance()
        {
            static Type instance;
            return &instance;
        }
    }
}