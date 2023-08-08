#include <Runtime/CodeObject.h>

#include <format>
#include <Exception/CoreException.h>


namespace XyA
{
    namespace Runtime
    {
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

        Object* CodeObject::get_literal(Index index) const
        {
            return this->literals[index];
        }

        StringView CodeObject::get_variale_name(Index index) const
        {
            return this->variable_names[index];
        }

        template <typename T, typename ValueT>
        bool CodeObject::try_get_literal_index(const ValueT& literal_value, Index& index_result) const
        {
            auto iter = std::find_if(
                this->literals.begin(), this->literals.end(),
                [&](Object* existing_literal_object) -> bool
                {
                    return existing_literal_object->is_instance(T::static_type()) &&
                        static_cast<T*>(existing_literal_object)->value == literal_value;
                }
            );

            if (iter == this->literals.end())
            {
                return false;
            }

            index_result = std::distance(this->literals.begin(), iter);
            return true;
        }

        template
        bool CodeObject::try_get_literal_index<Builtin::IntObject, Builtin::IntValue>(const Builtin::IntValue&, Index&) const;
        template
        bool CodeObject::try_get_literal_index<Builtin::FloatObject, Builtin::FloatValue>(const Builtin::FloatValue&, Index&) const;
        template
        bool CodeObject::try_get_literal_index<Builtin::StringObject, Builtin::StringValue>(const Builtin::StringValue&, Index&) const;
        template
        bool CodeObject::try_get_literal_index<Builtin::BoolObject, Builtin::BoolValue>(const Builtin::BoolValue&, Index&) const;

        bool CodeObject::try_get_variable_index(StringView variable_name, Index& index_result) const
        {
            auto iter = std::find(this->variable_names.begin(), this->variable_names.end(), variable_name);

            if (iter == this->variable_names.end())
            {
                return false;
            }

            index_result = std::distance(this->variable_names.begin(), iter);
            return true;
        }

        template <typename T>
        Index CodeObject::get_literal_index(T* literal_object)
        {
            Index index_result;

            if (this->try_get_literal_index<T>(literal_object->value, index_result))
            {
                return index_result;
            }
            return this->register_literal_object(literal_object);
        }

        template
        Index CodeObject::get_literal_index<Builtin::IntObject>(Builtin::IntObject*);
        template
        Index CodeObject::get_literal_index<Builtin::FloatObject>(Builtin::FloatObject*);
        template
        Index CodeObject::get_literal_index<Builtin::StringObject>(Builtin::StringObject*);
        template
        Index CodeObject::get_literal_index<Builtin::BoolObject>(Builtin::BoolObject*);

        Index CodeObject::get_variable_index(StringView variable_name)
        {
            Index index_result;

            if (this->try_get_variable_index(variable_name, index_result))
            {
                return index_result;
            }

            return this->register_variable_name(variable_name);
        }

        Index CodeObject::register_literal_object(Object* literal_object)
        {
            this->literals.push_back(literal_object);
            literal_object->ref_count = 1;
            return this->literals.size() - 1;
        }

        Index CodeObject::register_variable_name(StringView variable_name)
        {
            auto iter = std::find(
                this->variable_names.begin(), this->variable_names.end(), variable_name);
            
            if (iter != this->variable_names.end())
            {
                XyA_Throw_Core_Exception(std::format("Variable name '{}' alrady exists", variable_name));
            }

            this->variable_names.push_back(variable_name);
            return this->variable_names.size() - 1;
        }

        #ifdef Debug_Display_Object
        std::string CodeObject::to_string() const
        {
            return "Code Object";
        }
        #endif
    }
}