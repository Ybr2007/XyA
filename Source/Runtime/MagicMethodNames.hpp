#pragma once
#include <string>
#include <unordered_map>


namespace XyA
{
    namespace Runtime
    {
        namespace MagicMethodNames
        {
            const std::string add_method_name = "operator+";
            const std::string subtract_method_name = "operator-";
            const std::string multiply_method_name = "operator*";
            const std::string divide_method_name = "operator/";
            const std::string equal_method_name = "operator==";
            const std::string not_equal_method_name = "__ne__";
            const std::string greater_method_name = "operator>";
            const std::string greater_equal_method_name = "operator>=";
            const std::string less_method_name = "operator<";
            const std::string less_equal_method_name = "operator<=";
            const std::string bool_method_name = "__bool__";
            const std::string str_method_name = "__str__";
        }
    }
}