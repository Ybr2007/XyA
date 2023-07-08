#pragma once
#include <string>
#include <unordered_map>


namespace XyA
{
    namespace Runtime
    {
        namespace MagicMethodNames
        {
            const size_t magic_method_num = 12;
            const std::string add_method_name = "operator+";
            const size_t add_method_index = 0;
            const std::string subtract_method_name = "operator-";
            const size_t subtract_method_index = 1;
            const std::string multiply_method_name = "operator*";
            const size_t multiply_method_index = 2;
            const std::string divide_method_name = "operator/";
            const size_t divide_method_index = 3;
            const std::string equal_method_name = "operator==";
            const size_t equal_method_index = 4;
            const std::string not_equal_method_name = "__ne__";
            const size_t not_equal_method_index = 5;
            const std::string greater_method_name = "operator>";
            const size_t greater_method_index = 6;
            const std::string greater_equal_method_name = "operator>=";
            const size_t greater_equal_method_index = 7;
            const std::string less_method_name = "operator<";
            const size_t less_method_index = 8;
            const std::string less_equal_method_name = "operator<=";
            const size_t less_equal_method_index = 9;
            const std::string bool_method_name = "__bool__";
            const size_t bool_method_index = 10;
            const std::string str_method_name = "__str__";
            const size_t str_method_index = 11;

        const std::unordered_map<std::string, size_t> magic_method_indices = {
                {add_method_name, add_method_index}, 
                {subtract_method_name, subtract_method_index}, 
                {multiply_method_name, multiply_method_index}, 
                {divide_method_name, divide_method_index}, 
                {equal_method_name, equal_method_index}, 
                {not_equal_method_name, not_equal_method_index}, 
                {greater_method_name, greater_method_index}, 
                {greater_equal_method_name, greater_equal_method_index}, 
                {less_method_name, less_method_index}, 
                {less_equal_method_name, less_equal_method_index}, 
                {bool_method_name, bool_method_index}, 
                {str_method_name, str_method_index}, 
            };

            inline bool try_get_magic_method_index(const std::string& method_name, size_t& result)
            {
                auto iter = magic_method_indices.find(method_name);
                if (iter == magic_method_indices.end())
                {
                    return false;
                }
                result = iter->second;
                return true;
            }
        }
    }
}