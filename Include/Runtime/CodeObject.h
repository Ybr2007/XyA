#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <Runtime/Instruction.h>
#include <Runtime/Object.h>
#include <Runtime/Builtin/Int.h>
#include <Runtime/Builtin/Float.h>
#include <Runtime/Builtin/String.h>


namespace XyA
{
    namespace Runtime
    {
        class CustomFunction;
        
        class CodeObject : public Object
        {
        public:
            std::vector<Instruction*> instructions;  
            std::vector<Object*> literals;
            std::vector<std::pair<size_t, Object*>> prebuilt_objects;
            std::unordered_map<std::string, size_t> variable_name_2_index;
            std::vector<std::string> attr_names;
            Type* cls = nullptr;

            ~CodeObject();

            bool try_get_literal_index(Builtin::IntObject* literal_object, size_t& result) const;
            bool try_get_literal_index(Builtin::FloatObject* literal_object, size_t& result) const;
            bool try_get_literal_index(Builtin::StringObject* literal_object, size_t& result) const;
            bool try_get_literal_index(Builtin::BoolObject* literal_object, size_t& result) const;
            bool try_get_variable_index(const std::string& variable_name, size_t& result) const;
            bool try_get_attr_name_index(const std::string& attr_name, size_t& result) const;

            size_t add_literal_object(Object* literal_object);
            size_t add_variable_name(const std::string& variable_name);
            size_t add_attr_name(const std::string& attr_name);
            
            #ifdef Debug_Display_Object
            virtual std::string to_string() const;
            #endif
        };
        
    }
}