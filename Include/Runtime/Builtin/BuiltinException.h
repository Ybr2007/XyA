#pragma once
#include <Runtime/Object.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            class BuiltinException : public Object
            {
            public:
                std::string message;

                BuiltinException(std::string message);
            };
        }
    }
}