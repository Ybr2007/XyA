#pragma once
#include <Runtime/Object.h>
#include <Runtime/Builtin/Int.h>
#include <Runtime/Builtin/Null.h>
#include <Runtime/Builtin/String.h>
#include <Runtime/MagicMethodNames.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            Object* print(Object** args, size_t arg_num, bool& exception_thrown);
            Object* _get_ref_count(Object** args, size_t arg_num, bool& exception_thrown);
            Object* _get_id(Object** args, size_t arg_num, bool& exception_thrown);
            Object* clock_(Object** args, size_t arg_num, bool& exception_thrown);
            Object* sizeof_(Object** args, size_t arg_num, bool& exception_thrown);
        }
    }
}