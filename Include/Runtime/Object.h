#pragma once
#include <string>
#include <array>
#include <unordered_map>
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
        
        enum class TryGetAttrResult
        {
            OK,
            NotFound
        };

        class Object
        {
        public:
            Type* type = nullptr;
            long long ref_count = 0;
            std::array<Object*, MagicMethodNames::magic_method_num> magic_methods{};
            std::unordered_map<std::string, Object*> attrs;

            void reference();
            bool dereference();
            bool deallocate_if_no_ref();

            virtual void reference_attrs();
            virtual void dereference_attrs();

            bool is_instance(Type* type) const;

            TryGetMethodResult try_get_magic_method(size_t index, BaseFunction*& result) const;
            TryGetAttrResult try_get_attr(const std::string& attr_name, Object*& result) const;

            virtual ~Object();

            #ifdef Debug_Display_Object
            virtual std::string to_string() const;
            #endif        
        };

        class Type : public Object
        {
        public:
            std::string name;
            std::array<Object*, MagicMethodNames::magic_method_num> instance_magic_methods{};
            std::unordered_map<std::string, Object*> instance_methods;
            bool instance_allows_external_attr = true;

            void reference_attrs() override;
            virtual ~Type() override;
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