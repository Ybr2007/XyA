#pragma once
#include <string>
#include <array>
#include <unordered_map>
#include <Runtime/MagicMethodNames.hpp>
#include <Utils/StrKeyDict.hpp>
#include <Config.h>


namespace XyA
{
    namespace Runtime
    {
        class Type;
        class BaseFunction;
        class Object;

        enum class TryGetAttrResult
        {
            OK,
            NotFound
        };

        enum class TryGetMethodResult
        {
            OK,
            NotFound,
            NotCallable,
        };

        class Object
        {
        public:
            long long ref_count = 0;
            StrKeyDict<Object*> attrs;

            Type* type() const;
            void set_type(Type* type);

            void reference();
            bool dereference();
            bool deallocate_if_no_ref();

            void reference_attrs();
            void dereference_attrs();

            bool is_instance(Type* type) const;
            TryGetAttrResult try_get_attr(const std::string& attr_name, Object*& result) const;
            TryGetMethodResult try_get_method(const std::string& method_name, BaseFunction*& result) const;
            virtual ~Object();

            #ifdef Debug_Display_Object
            virtual std::string to_string() const;
            #endif   

        protected:
            Type* __type = nullptr;
        };

        class Type : public Object
        {
        public:
            std::string name = "type";
            bool instance_allow_external_attr = false;

            static Type* get_instance();
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