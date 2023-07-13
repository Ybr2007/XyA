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
                BuiltinException(std::string message);

                std::string_view message() const override final;

            private:
                std::string_view __message;
            };
        }
    }
}