#pragma once
#include <vector>
#include <utility>
#include <TypeDefs.h>
#include <Runtime/Instruction.h>
#include <Runtime/Object.h>
#include <Runtime/Type.h>
#include <Runtime/Builtin/Int.h>
#include <Runtime/Builtin/Float.h>
#include <Runtime/Builtin/String.h>


namespace XyA
{
    namespace Runtime
    {
        class CustomFunction;

        class CodeObject : public Object
        {
        public:
            // 指令序列
            std::vector<Instruction*> instructions;

            // 字面量
            std::vector<Object*> literals;

            /*
                预构建的对象，在从CodeObject创建Context时，
                会将这些对象设置到Context::local_variables
                的指定位置
            */
            std::vector<std::pair<Index, Object*>> prebuilt_objects;

            /*
                被导入模块的数据，pair::first是模块的CodeObject，
                pair::second是模块变量对应的Context::local_variables
                的Index
            */
            std::vector<std::pair<CodeObject*, Index>> module_datas;

            std::vector<StringView> variable_names;

            Type* cls = nullptr;

            ~CodeObject();

            Object* get_literal(Index index) const;
            StringView get_variale_name(Index index) const;

            template <typename T, typename ValueT>
            bool try_get_literal_index(const ValueT& int_literal_value, Index& index_result) const;

            bool try_get_variable_index(StringView variable_name, Index& index_result) const;

            template <typename T>
            Index get_literal_index(T* literal_object);
            Index get_variable_index(StringView variable_name);

            Index register_literal_object(Object* literal_object);
            Index register_variable_name(StringView variable_name);
            
            #ifdef Debug_Display_Object
            virtual std::string to_string() const;
            #endif
        };
        
    }
}