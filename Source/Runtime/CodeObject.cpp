#pragma once
#include <Runtime/CodeObject.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        CodeObject::~CodeObject()
        {
            for (size_t i = 0; i < this->literals.size(); i ++)
            {
                if (!XyA_Check_If_Deallocated(this->literals[i]))
                {
                    // printf("Deallocate Literal XXX: %s\n", this->literals[i]->to_string().c_str());
                    this->literals[i]->dereference();
                }
            }

            for (Instruction* instruction : this->instructions)
            {
                delete instruction;
            }
        }

        bool CodeObject::try_get_literal_index(Builtin::IntObject* literal_object, size_t& result) const
        {
            auto it = std::find_if(literals.begin(), literals.end(),
                [&](Object* obj) 
                {
                    auto int_obj = dynamic_cast<Builtin::IntObject*>(obj);
                    return int_obj && int_obj->value == literal_object->value;
                }
            );
            if (it != literals.end()) 
            {
                result = std::distance(literals.begin(), it);
                return true;
            } 
            else 
            {
                return false;
            }
        }

        bool CodeObject::try_get_literal_index(Builtin::FloatObject* literal_object, size_t& result) const
        {
            auto it = std::find_if(literals.begin(), literals.end(),
                [&](Object* obj) 
                {
                    auto defined_obj = dynamic_cast<Builtin::FloatObject*>(obj);
                    return defined_obj && defined_obj->value == literal_object->value;
                }
            );
            if (it != literals.end()) 
            {
                result = std::distance(literals.begin(), it);
                return true;
            } 
            else 
            {
                return false;
            }
        }

        bool CodeObject::try_get_literal_index(Builtin::StringObject* literal_object, size_t& result) const
        {
            auto it = std::find_if(literals.begin(), literals.end(),
                [&](Object* obj) 
                {
                    auto defined_obj = dynamic_cast<Builtin::StringObject*>(obj);
                    return defined_obj && defined_obj->value == literal_object->value;
                }
            );
            if (it != literals.end()) 
            {
                result = std::distance(literals.begin(), it);
                return true;
            } 
            else 
            {
                return false;
            }
        }

        bool CodeObject::try_get_literal_index(Builtin::BoolObject* literal_object, size_t& result) const
        {
            auto it = std::find_if(literals.begin(), literals.end(),
                [&](Object* obj) 
                {
                    auto defined_obj = dynamic_cast<Builtin::BoolObject*>(obj);
                    return defined_obj && defined_obj->value == literal_object->value;
                }
            );
            if (it != literals.end()) 
            {
                result = std::distance(literals.begin(), it);
                return true;
            } 
            else 
            {
                return false;
            }
        }

        bool CodeObject::try_get_variable_index(const std::string& variable_name, size_t& result) const
        {
            auto it = this->variable_name_indices.find(variable_name);
            if (it != variable_name_indices.end()) 
            {
                result = it->second;
                return true;
            } 
            else 
            {
                return false;
            }
        }

        size_t CodeObject::add_literal_object(Object* literal_object)
        {
            this->literals.push_back(literal_object);
            literal_object->ref_count = 1;
            return this->literals.size() - 1;
        }

        size_t CodeObject::add_variable_name(const std::string& variable_name)
        {
            size_t index = this->variable_name_indices.size();
            this->variable_name_indices[variable_name] = index;
            return index;
        }

        #ifdef Debug_Display_Object
        std::string CodeObject::to_string() const
        {
            return "Code Object";
        }
        #endif
    }
}