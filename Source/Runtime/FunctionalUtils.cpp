#include <Runtime/FunctionalUtils.h>

namespace XyA
{
    namespace Runtime
    {
        bool is_function(Object* object)
        {
            return is_builtin_function(object) || is_custom_function(object);
        }

        bool is_builtin_function(Object* object)
        {
            return object->type() == Builtin::BuiltinFunctionType::get_instance();
        }

        bool is_custom_function(Object* object)
        {
            return object->type() == FunctionType::get_instance();
        }
    }
}