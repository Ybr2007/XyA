#include <Exception/CoreException.h>


namespace XyA
{
    CoreException::CoreException()
    {
        this->__message = "Core Error";
    }

    CoreException::CoreException(std::string_view message)
    {
        this->__message = message;
    }

    const char* CoreException::what() const throw()
    {
        return this->__message.data();
    }
}