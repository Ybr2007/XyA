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
            size_t ref_count = 0;
            std::unordered_map<std::string, Object*> attrs;

            bool dwindle_ref_count();
            TryGetMethodResult try_get_method(const std::string& method_name, BaseFunction*& result) const;
            virtual ~Object();
        };

        Object::~Object()
        {
            printf("Delete\n");
        }
        
        class Type : public Object
        {
        public:
            std::string name;
        };

        bool Object::dwindle_ref_count()
        {
            if (-- this->ref_count <= 0)
            {
                #ifndef DebugMode
                delete this;
                #endif
                return true;
            }
            return false;
        }

        class BaseFunction : public Object
        {
        public:
            virtual Object* call(Object** args, size_t arg_num, bool& exception_thrown) const = 0;
        };

        TryGetMethodResult Object::try_get_method(const std::string& method_name, BaseFunction*& result) const
        {
            auto iter = this->attrs.find(method_name);
            if (iter == this->attrs.end())
            {
                iter = this->type->attrs.find(method_name);
                if (iter == this->type->attrs.end())
                {
                    return TryGetMethodResult::NotFound;
                }
            }
            BaseFunction* result_buffer = dynamic_cast<BaseFunction*>(iter->second);
            if (result_buffer == nullptr)
            {
                return TryGetMethodResult::NotCallable;
            }
            result = result_buffer;
            return TryGetMethodResult::OK;
        }
    }
}