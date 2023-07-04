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
            for (auto attr : this->attrs)
            {
                attr->reference();
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
            for (auto attr : this->attrs)
            {
                attr->dereference();
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

        TryGetMethodResult Object::try_get_magic_method(size_t index, BaseFunction*& result) const
        {
            Object* magic_method = this->magic_methods[index];
            if (magic_method == nullptr)
            {
                magic_method = this->type->magic_methods[index];
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
        
        TryGetMethodResult Object::try_get_method(const std::string& method_name, BaseFunction*& result) const
        {
            size_t magic_method_index;
            bool is_magic_method = MagicMethodNames::try_get_magic_method_index(method_name, magic_method_index);

            if (is_magic_method)
            {
                Object* magic_method = this->magic_methods[magic_method_index];
                if (magic_method == nullptr)
                {
                    magic_method = this->type->magic_methods[magic_method_index];
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

            for (size_t i = 0; i < this->attr_names.size(); i ++)
            {
                if (this->attr_names[i] == method_name)
                {
                    result = dynamic_cast<BaseFunction*>(this->attrs[i]);
                    if (result != nullptr)
                    {
                        return TryGetMethodResult::OK;
                    }
                    else
                    {
                        return TryGetMethodResult::NotCallable;
                    }
                }
            }

            if (this->type == nullptr)
            {
                return TryGetMethodResult::NotFound;
            }

            for (size_t i = 0; i < this->type->attr_names.size(); i ++)
            {
                if (this->type->attr_names[i] == method_name)
                {
                    result = dynamic_cast<BaseFunction*>(this->type->attrs[i]);
                    if (result != nullptr)
                    {
                        ;
                        return TryGetMethodResult::OK;
                    }
                    else
                    {
                        return TryGetMethodResult::NotCallable;
                    }
                }
            }

            return TryGetMethodResult::NotFound;
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