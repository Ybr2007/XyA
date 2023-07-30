/*
    # Name Mapping
    Name Mapping 是 XyA 处理如属性名(Attribute Name)等名称的机制
    NameMapper 会为每一个名称指定一个唯一的 id，该 id 代替名称 string 作为指令的参数和 Object::attrs 的 key
    注意：Name Mapping 机制不处理变量名(Variable Name)，因为变量名的 id 可以在当前上下文中确定，能直接在 CodeObject 的编译时内部分配
*/

#pragma once
#include <optional>
#include <unordered_map>
#include <TypeDefs.h>


namespace XyA
{
    namespace Runtime
    {

        class NameMapper
        {
        public:
            static NameMapper& get_instance();

            void register_name(StringView name);

            Id get_name_id(StringView name);

            std::optional<Id> try_get_name_id(StringView name) const;

            StringView get_name(Id id) const;

        private:
            std::unordered_map<StringView, Id> __map;

            NameMapper();
            NameMapper(const NameMapper& other) = delete;
            NameMapper& operator=(const NameMapper& other) = delete;
        };

    }
}