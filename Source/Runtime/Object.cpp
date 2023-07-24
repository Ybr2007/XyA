#include <Runtime/Object.h>
#include <Runtime/Type.h>
#include <Runtime/Builtin/BuiltinFunction.h>
#include <Runtime/CustomFunction.h>
#include <Runtime/MemoryManager.hpp>
#include <Runtime/FunctionalUtils.hpp>


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

        void Object::set_type(Type* type)
        {
            this->__type = type;
        }

        void Object::reference()
        {
            if (!this->ref_count_enabled)
            {
                return;
            }
            this->ref_count ++;
        }

        bool Object::dereference()
        {
            if (!this->ref_count_enabled)
            {
                return false;
            }

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
            if (!this->ref_count_enabled)
            {
                return false;
            }

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
                attr->value.object->reference();
            }
        }

        void Object::dereference_attrs()
        {
            for (auto attr : this->attrs)
            {
                attr->value.object->dereference();
            }
        }

        bool Object::is_instance(Type* type) const
        {
            return this->type() == type;
        }

        void Object::set_attr(size_t attr_name_id, Attr attr)
        {
            Attr old_attr;
            if (this->attrs.try_get(attr_name_id, old_attr))
            {
                old_attr.object->dereference();
            }
            attr.object->reference();
            this->attrs[attr_name_id] = attr;
        }

        void Object::set_attr(size_t attr_name_id, Object* attr_object, AttrAccessibility accessibility)
        {
            Attr old_attr;
            if (this->attrs.try_get(attr_name_id, old_attr))
            {
                old_attr.object->dereference();
            }

            Attr attr;
            attr.object = attr_object;
            attr_object->reference();
            attr.accessibility = accessibility;
            this->attrs[attr_name_id] = attr;
        }

        TryGetAttrResult Object::try_get_attr(size_t attr_name_id, Attr& result) const
        {
            if (this->attrs.try_get(attr_name_id, result))
            { 
                return TryGetAttrResult::OK;
            }
            return this->__type == nullptr ? TryGetAttrResult::NotFound : this->__type->try_get_attr(attr_name_id, result);
        }

        TryGetMethodResult Object::try_get_method(
            size_t method_name_id, BaseFunction*& method_result, AttrAccessibility& accessibility_result) const
        {
            Attr attr;
            auto operation_result = this->try_get_attr(method_name_id, attr);
            if (operation_result == TryGetAttrResult::NotFound)
            {
                return TryGetMethodResult::NotFound;
            }
            if (is_function(attr.object))
            {
                method_result = static_cast<BaseFunction*>(attr.object);
                accessibility_result = attr.accessibility;
                return TryGetMethodResult::OK;
            }
            return TryGetMethodResult::NotCallable;
        }

        #ifdef Debug_Display_Object
        std::string Object::to_string() const
        {
            return this->__type == nullptr ? 
                "<XyA Object at " + std::to_string((size_t)this) + ">" : 
                "<" + this->__type->name + " Object at " + std::to_string((size_t)this) + ">" ;
        }
        #endif
        
    }
}