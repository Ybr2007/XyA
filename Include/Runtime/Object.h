#pragma once
#include <string>
#include <array>
#include <vector>
#include <Runtime/MagicMethodNames.hpp>
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
            std::array<Object*, MagicMethodNames::magic_method_num> magic_methods{};
            std::vector<Object*> attrs;
            std::vector<std::string> attr_names;

            void reference();
            bool dereference();
            bool deallocate_if_no_ref();

            void reference_attrs();
            void dereference_attrs();

            bool is_instance(Type* type) const;
            TryGetMethodResult try_get_magic_method(size_t index, BaseFunction*& result) const;
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

        #define XyA_Function_Check_Arg_Num(expected_arg_num_) \
                if (arg_num != expected_arg_num_) \
                { \
                    exception_thrown = true; \
                    return new Builtin::BuiltinException("Expected " + std::to_string(expected_arg_num_) + " arguments, got " + std::to_string(arg_num)); \
                }

        class BaseFunction : public Object
        {
        public:
            virtual Object* call(Object** args, size_t arg_num, bool& exception_thrown) const = 0;
        };
    }
}