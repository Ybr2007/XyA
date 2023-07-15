#pragma once
#include <format>
#include <string>
#include <exception>
#include <Utils/CodeLocation.h>


namespace XyA
{
    #define XyA_Throw_Core_Exception(message) throw CoreException(std::format("{} {}", message, FILE_LINE))

    class CoreException : public std::exception
    {
    public:
        CoreException();
        CoreException(std::string_view msg);
        virtual const char* what() const throw();

    protected:
        std::string_view __message;
    };
}