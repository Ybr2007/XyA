#include <Runtime/Type.h>
#include <iostream>


namespace XyA
{
    namespace Runtime
    {

        Type::Type() : name("type")  // public
        {

        }

        Type::Type(StringView name_) : name(name_)  // public
        {
            
        }

        Type* Type::get_instance()
        {
            static Type instance;
            return &instance;
        }

    }
}