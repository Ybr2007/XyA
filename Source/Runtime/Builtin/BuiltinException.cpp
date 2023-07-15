#include <Runtime/Builtin/BuiltinException.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            BuiltinException::BuiltinException(std::string message)
            {
                this->__message = message;
            }
            
            std::string_view BuiltinException::message() const
            {
                return this->__message;
            }
        }
    }
}