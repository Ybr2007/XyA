#pragma once
#include <Runtime/Builtin/BuiltinException.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            BuiltinException::BuiltinException(std::string message)
            {
                this->message = message;
            }
            
        }
    }
}