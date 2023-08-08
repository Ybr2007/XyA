#pragma once
#include <Runtime/Object.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            class BuiltinException : public BaseException
            {
            public:
                BuiltinException(StringView message);

                StringView message() const override final;

            private:
                StringSource __message;
            };
        }
    }
}