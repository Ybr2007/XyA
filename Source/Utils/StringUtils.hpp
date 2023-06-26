#pragma once
#include <string>


namespace XyA
{
    namespace Utils
    {
        inline bool is_letter(char chr)
        {
            return ('a' <= chr && chr <= 'z') || ('A' <= chr && chr <= 'Z');
        }

        inline bool is_digit(char chr)
        {
            return '0' <= chr && chr <= '9';
        }
    }
}