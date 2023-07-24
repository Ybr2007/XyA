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
            const std::string as_method_name = "__as__";

            const size_t add_method_name_id = 0;
            const size_t subtract_method_name_id = 1;
            const size_t multiply_method_name_id = 2;
            const size_t divide_method_name_id = 3;
            const size_t equal_method_name_id = 4;
            const size_t not_equal_method_name_id = 5;
            const size_t greater_method_name_id = 6;
            const size_t greater_equal_method_name_id = 7;
            const size_t less_method_name_id = 8;
            const size_t less_equal_method_name_id = 9;
            const size_t bool_method_name_id = 10;
            const size_t str_method_name_id = 11;
            const size_t new_method_name_id = 12;
            const size_t init_method_name_id = 13;
            const size_t as_method_name_id = 14;
        }
    }
}