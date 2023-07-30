#include <Runtime/NameMapping.h>

#include <format>
#include <Exception/CoreException.h>
#include <Runtime/MagicMethodNames.h>


namespace XyA
{
    namespace Runtime
    {

        NameMapper& NameMapper::get_instance()
        {
            static NameMapper instance;
            return instance;
        }

        void NameMapper::register_name(StringView name)
        {
            size_t id = this->__map.size();
            this->__map[name] = id;
        }

        size_t NameMapper::get_name_id(StringView name)
        {
            auto optional_id = this->try_get_name_id(name);
            if (optional_id.has_value())
            {
                return optional_id.value();
            }

            size_t id = this->__map.size();
            this->__map[name] = id;
            return id;
        }

        std::optional<size_t> NameMapper::try_get_name_id(StringView name) const
        {
            auto iter = this->__map.find(name);
            if (iter == this->__map.end())
            {
                return std::nullopt;
            }
            return iter->second;
        }

        StringView NameMapper::get_name(size_t id) const
        {
            for (auto iter : this->__map)
            {
                if (iter.second == id)
                {
                    return iter.first;
                }
            }
            XyA_Throw_Core_Exception(std::format("Invalid id '{}'", id));
        }

        NameMapper::NameMapper()  // private
        {
            // 在NameMapper被构造时，先将Magic Methods的名称注册
            this->register_name(MagicMethodNames::add_method_name);
            this->register_name(MagicMethodNames::subtract_method_name);
            this->register_name(MagicMethodNames::multiply_method_name);
            this->register_name(MagicMethodNames::divide_method_name);
            this->register_name(MagicMethodNames::equal_method_name);
            this->register_name(MagicMethodNames::not_equal_method_name);
            this->register_name(MagicMethodNames::greater_method_name);
            this->register_name(MagicMethodNames::greater_equal_method_name);
            this->register_name(MagicMethodNames::less_method_name);
            this->register_name(MagicMethodNames::less_equal_method_name);
            this->register_name(MagicMethodNames::bool_method_name);
            this->register_name(MagicMethodNames::str_method_name);
            this->register_name(MagicMethodNames::new_method_name);
            this->register_name(MagicMethodNames::init_method_name);
            this->register_name(MagicMethodNames::as_method_name);
        }

    }
}