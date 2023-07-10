#pragma once
#include <string>
#include <unordered_map>


namespace XyA
{
    namespace Runtime
    {
        namespace MagicMethodNames
        {
            const std::string add_method_name = "__add__";
            const std::string subtract_method_name = "__sub__";
            const std::string multiply_method_name = "__mul__";
            const std::string divide_method_name = "__div__";
            const std::string equal_method_name = "__eq__";
            const std::string not_equal_method_name = "__ne__";
            const std::string greater_method_name = "__gt__";
            const std::string greater_equal_method_name = "__ge__";
            const std::string less_method_name = "__lt__";
            const std::string less_equal_method_name = "__le__";
            const std::string bool_method_name = "__bool__";
            const std::string str_method_name = "__str__";
            const std::string new_method_name = "__new__";
            const std::string init_method_name = "__init__";
        }
    }
}