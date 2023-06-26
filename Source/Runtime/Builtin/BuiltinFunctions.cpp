#pragma once
#include <Runtime/Builtin/BuiltinFunctions.h>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            /* 模拟函数退出时（Context结束时）所有object ref_count减一 */
            inline void _builtin_function_end(Object** args, size_t arg_num)
            {
                for (size_t i = 0; i < arg_num; i ++)
                {
                    args[i]->dwindle_ref_count();
                }
            }

            Object* print(Object** args, size_t arg_num, bool& exception_thrown)
            {
                for (size_t i = 0; i < arg_num; i ++)
                {
                    Builtin::StringObject* str_obj = dynamic_cast<Runtime::Builtin::StringObject*>(args[i]);

                    bool new_object = false;
                    if (str_obj == nullptr)
                    {
                        Runtime::BaseFunction* str_method; 
                        auto result = args[i]->try_get_method(MagicMethodNames::str_method_name, str_method);
                        if (result == TryGetMethodResult::NotFound || result == TryGetMethodResult::NotCallable)
                        {
                            str_obj = new Builtin::StringObject;
                            str_obj->value = "<XyA Object at " + std::to_string((unsigned long long)args[i]) + ">";
                            new_object = true;
                        }
                        else
                        {
                            Object** str_method_args = new Object*[1]{args[i]};
                            Object* result = str_method->call(str_method_args, 1, exception_thrown);
                            delete str_method_args;

                            if (exception_thrown)
                            {
                                _builtin_function_end(args, arg_num);
                                return result;
                            }

                            str_obj = dynamic_cast<Builtin::StringObject*>(result);
                        }
                    }
                    printf("%s TYPE: %s\n", str_obj->value.c_str(), args[i]->type->name.c_str());

                    if (new_object)
                    {
                        delete str_obj;
                    }
                }

                _builtin_function_end(args, arg_num);
                return NullObject::get_instance();
            }

            Object* _get_ref_count(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 1)
                {
                    exception_thrown = true;
                    _builtin_function_end(args, arg_num);
                    return new BuiltinException("Excepted 1 argument, got " + std::to_string(arg_num));
                }

                IntObject* result = new IntObject;
                result->value = args[0]->ref_count;

                _builtin_function_end(args, arg_num);
                return result;
            }

            Object* _get_id(Object** args, size_t arg_num, bool& exception_thrown)
            {
                if (arg_num != 1)
                {
                    exception_thrown = true;
                    _builtin_function_end(args, arg_num);
                    return new BuiltinException("Excepted 1 argument, got " + std::to_string(arg_num));
                }

                IntObject* result = new IntObject;
                result->value = (long long)args[0];

                _builtin_function_end(args, arg_num);
                return result;
            }
        }
    }
}