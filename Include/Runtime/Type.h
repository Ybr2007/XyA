#pragma once
#include <TypeDefs.h>
#include <Runtime/Object.h>


namespace XyA
{
    namespace Runtime
    {

        class Type : public Object
        {
        public:
            StringView name;
            bool allow_ext_attr_add = false;

            Type();
            Type(StringView name);

            static Type* get_instance();
        };

    }
}