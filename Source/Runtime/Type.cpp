#include <Runtime/Type.h>

namespace XyA
{
    namespace Runtime
    {

        Type::Type() : name("type")  // public
        {

        }

        Type::Type(std::string_view name_) : name(name_)  // public
        {

        }

        Type* Type::get_instance()
        {
            static Type instance;
            return &instance;
        }

    }
}