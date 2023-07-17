#pragma once
#include <string>
#include <array>
#include <unordered_map>
#include <format>
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
            NotFound,
        };

        enum class TryGetMethodResult
        {
            OK,
            NotFound,
            NotCallable,
        };

        struct ObjectRef
        {
            Object* object;
        };

        enum AttrAccessibility
        {
            Public,
            Private,
        };

        struct Attr : public ObjectRef
        {
            AttrAccessibility accessibility;
        };

        class Object
        {
        public:
            StrKeyDict<Attr> attrs;
            long long ref_count = 0;
            bool ref_count_enabled = true;

            Type* type() const;
            void set_type(Type* type);

            void reference();
            bool dereference();
            bool deallocate_if_no_ref();

            void reference_attrs();
            void dereference_attrs();

            bool is_instance(Type* type) const;

            void set_attr(const std::string& attr_name, Attr attr);
            void set_attr(const std::string& attr_name, Object* attr_object, AttrAccessibility accessibility = AttrAccessibility::Public);
            TryGetAttrResult try_get_attr(const std::string& attr_name, Attr& result) const;
            TryGetMethodResult try_get_method(
                const std::string& method_name, BaseFunction*& method_result, AttrAccessibility& accessibility_result) const;
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
            bool allow_ext_attr_add = false;

            static Type* get_instance();
        };

        #define XyA_Function_Check_Arg_Num(expected_arg_num_) \
                if (arg_num != expected_arg_num_) \
                { \
                    exception_thrown = true; \
                    return XyA_Allocate(Builtin::BuiltinException, \
                        std::format("Expected {} arguments, got {}", expected_arg_num_, arg_num)); \
                }

        class BaseFunction : public Object
        {
        public:
            virtual Object* call(Object** args, size_t arg_num, bool& exception_thrown) const = 0;
        };
        
        class BaseException : public Object
        {
        public:
            virtual std::string_view message() const = 0;
        };
    }
}