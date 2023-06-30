#pragma once
#include <string>
#include <unordered_map>
#include <Config.h>


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
            Type* type = nullptr;
            long long ref_count = 0;
            std::unordered_map<std::string, Object*> attrs;

            void reference();

            bool dereference();
            TryGetMethodResult try_get_method(const std::string& method_name, BaseFunction*& result) const;
            virtual ~Object();

            #ifdef Debug_Display_Object
            virtual std::string to_string() const;
            #endif
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