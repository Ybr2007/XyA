#pragma once
#include <Runtime/CodeObject.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        CodeObject::CodeObject()
        {
            using T = std::unordered_map<std::string, Function*>;
            this->functions = XyA_Allocate_(T);
        }

        CodeObject::~CodeObject()
        {
            for (size_t i = 0; i < this->literals.size(); i ++)
            {
                this->literals[i]->dereference();
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
            auto it = this->variable_name_2_index.find(variable_name);
            if (it != variable_name_2_index.end()) 
            {
                result = it->second;
                return true;
            } 
            else 
            {
                return false;
            }
        }
        
        bool CodeObject::try_get_name_index(const std::string& name, size_t& result) const
        {
            auto it = this->variable_name_2_index.find(name);
            if (it != variable_name_2_index.end()) 
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
            size_t index = this->variable_name_2_index.size();
            this->variable_name_2_index[variable_name] = index;
            return index;
        }

        size_t CodeObject::add_name(const std::string& name)
        {
            this->names.push_back(name);
            return this->names.size() - 1;
        }

        #ifdef Debug_Display_Object
        std::string CodeObject::to_string() const
        {
            return "Code Object";
        }
        #endif
    }
}