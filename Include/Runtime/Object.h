#pragma once
#include <string>
#include <unordered_map>
#include <Config.hpp>


namespace XyA
{
    namespace Runtime
    {
        class Type;
        class BaseFunction;

        enum class TryGetMethodResult
        {
            OK,
            NotFound,
            NotCallable,
        };

        class Object
        {
        public:
            Type* type;
            long long ref_count = 0;
            std::unordered_map<std::string, Object*> attrs;

            bool dwindle_ref_count();
            TryGetMethodResult try_get_method(const std::string& method_name, BaseFunction*& result) const;
            virtual ~Object();
        };

        class Type : public Object
        {
        public:
            std::string name;
        };

        class BaseFunction : public Object
        {
        public:
            virtual Object* call(Object** args, size_t arg_num, bool& exception_thrown) const = 0;
        };
    }
}