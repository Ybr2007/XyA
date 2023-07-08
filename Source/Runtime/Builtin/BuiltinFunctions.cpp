#pragma once
#include <Runtime/Builtin/BuiltinFunctions.h>
#include <time.h>
#include <Runtime/MemoryManager.hpp>


namespace XyA
{
    namespace Runtime
    {
        namespace Builtin
        {
            Object* print(Object** args, size_t arg_num, bool& exception_thrown)
            {
                for (size_t i = 0; i < arg_num; i ++)
                {
                    Builtin::StringObject* str_obj = dynamic_cast<Runtime::Builtin::StringObject*>(args[i]);

                    bool object_is_string = (str_obj != nullptr);
                    if (!object_is_string)
                    {
                        Runtime::BaseFunction* str_method; 
                        auto result = args[i]->try_get_magic_method(MagicMethodNames::str_method_index, str_method);
                        if (result == TryGetMethodResult::NotFound || result == TryGetMethodResult::NotCallable)
                        {
                            str_obj = XyA_Allocate_(StringObject);
                            str_obj->value = "<XyA Object at " + std::to_string((unsigned long long)args[i]) + ">";
                        }
                        else
                        {
                            Object** str_method_args = new Object*[1]{args[i]};
                            Object* result = str_method->call(str_method_args, 1, exception_thrown);
                            delete[] str_method_args;

                            if (exception_thrown)
                            {
                                
                                return result;
                            }

                            str_obj = dynamic_cast<Builtin::StringObject*>(result);
                        }
                    }
                    printf("%s%s", str_obj->value.c_str(), i == arg_num - 1 ? "" : " ");
                    
                    if (!object_is_string)
                    {
                        XyA_Deallocate(str_obj);
                    }
                }
                printf("\n");
                
                return XyA_Allocate_(NullObject);
            }

            Object* _get_ref_count(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)

                IntObject* result = XyA_Allocate_(IntObject);
                result->value = args[0]->ref_count;

                
                return result;
            }

            Object* _get_id(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)

                IntObject* result = XyA_Allocate_(IntObject);
                result->value = (long long)args[0];
                
                return result;
            }

            Object* clock_(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(0)

                IntObject* result = XyA_Allocate_(IntObject);
                result->value = clock();
                
                return result;
            }

            Object* sizeof_(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)

                IntObject* result = XyA_Allocate_(IntObject);
                result->value = sizeof(*args[0]);
                
                return result;
            }
        }
    }
}