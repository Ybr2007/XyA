#pragma once
#include <Runtime/Object.h>

namespace XyA
{
    namespace Runtime
    {

        class Type : public Object
        {
        public:
            std::string_view name;
            bool allow_ext_attr_add = false;

            Type();
            Type(std::string_view name);

            static Type* get_instance();
        };

    }
}