#include <Runtime/Builtin/BuiltinException.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            BuiltinException::BuiltinException(StringView message)
            {
                this->__message = message;
            }
            
            StringView BuiltinException::message() const
            {
                return this->__message;
            }
        }
    }
}