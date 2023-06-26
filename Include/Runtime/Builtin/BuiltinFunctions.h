#pragma once
#include <Runtime/Object.h>
#include <Runtime/Builtin/Int.h>
#include <Runtime/Builtin/Null.h>
#include <Runtime/Builtin/String.h>
#include <Runtime/MagicMethodNames.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            /* 模拟函数退出时（Context结束时）所有object ref_count减一 */
            inline void _builtin_function_end(Object** args, size_t arg_num);

            Object* print(Object** args, size_t arg_num, bool& exception_thrown);

            Object* _get_ref_count(Object** args, size_t arg_num, bool& exception_thrown);

            Object* _get_id(Object** args, size_t arg_num, bool& exception_thrown);
        }
    }
}