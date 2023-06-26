#pragma once
#include <Runtime/Object.hpp>


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

            BuiltinException::BuiltinException(std::string message)
            {
                this->message = message;
            }
        }
    }
}