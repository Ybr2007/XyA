#pragma once
#include <string>
#include <array>
#include <unordered_map>
#include <format>
#include <TypeDefs.h>
#include <Runtime/MagicMethodNames.h>
#include <Utils/IdKeyDict.hpp>
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

        enum AttrAccessibility
        {
            Public,
            Private,
        };

        struct Attr
        {
            Object* object;
            AttrAccessibility accessibility;
        };

        class Object
        {
        public:
            IdKeyDict<Attr> attrs;
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

            void set_attr(Id attr_name_id, Attr attr);
            void set_attr(Id attr_name_id, Object* attr_object, AttrAccessibility accessibility = AttrAccessibility::Public);
            TryGetAttrResult try_get_attr(Id attr_name_id, Attr& result) const;
            TryGetMethodResult try_get_method(Id method_name_id, BaseFunction*& method_result, AttrAccessibility& accessibility_result) const;
            virtual ~Object();

            #ifdef Debug_Display_Object
            virtual std::string to_string() const;
            #endif   

        protected:
            Type* __type = nullptr;
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
            virtual StringView message() const = 0;
        };
    }
}