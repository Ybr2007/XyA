#include <format>
#include <chrono>
#include <Runtime/Builtin/BuiltinFunctions.h>
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
                    bool is_string = args[i]->is_instance(StringType::get_instance());
                    Builtin::StringObject* string_object = is_string ? static_cast<StringObject*>(args[i]) : nullptr;

                    bool new_string = false;
                    if (!is_string)
                    {
                        Runtime::BaseFunction* str_method; 
                        AttrAccessibility accessibility;
                        auto result = args[i]->try_get_method(MagicMethodNames::str_method_name, str_method, accessibility);
                        if (result == TryGetMethodResult::NotFound || result == TryGetMethodResult::NotCallable)
                        {
                            string_object = XyA_Allocate_(StringObject);
                            string_object->value = "<XyA Object at " + std::to_string((unsigned long long)args[i]) + ">";
                            new_string = true;
                        }
                        else
                        {
                            Object** str_method_args = XyA_Allocate_Array(Object*, 1, args[i]);
                            Object* str_method_return_value = str_method->call(str_method_args, 1, exception_thrown);
                            XyA_Deallocate_Array(str_method_args, 1);

                            if (exception_thrown)
                            {
                                return str_method_return_value;
                            }

                            if (!str_method_return_value->is_instance(StringType::get_instance()))
                            {
                                exception_thrown = true;
                                return XyA_Allocate(BuiltinException, 
                                    std::format("The return value of method '__str__' is not of str type, but of '{}' type", str_method_return_value->type()->name));
                            }

                            string_object = static_cast<Builtin::StringObject*>(str_method_return_value);
                        }
                    }
                    printf("%s%s", string_object->value.c_str(), i == arg_num - 1 ? "" : " ");
                    
                    if (new_string)
                    {
                        XyA_Deallocate(string_object);
                    }
                    else
                    {
                        string_object->deallocate_if_no_ref();
                    }
                }
                printf("\n");
                
                return NullObject::get_instance();
            }

            Object* _get_ref_count(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                
                return XyA_Allocate(IntObject, args[0]->ref_count);
            }

            Object* _get_id(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)

                return XyA_Allocate(IntObject, (long long)args[0]);
            }


            auto start_time = std::chrono::high_resolution_clock::now();

            Object* clock_(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(0)

                auto cur_time = std::chrono::high_resolution_clock::now();
                auto t = std::chrono::duration_cast<std::chrono::nanoseconds>(cur_time - start_time);

                return XyA_Allocate(IntObject, t.count());
            }

            Object* sizeof_(Object** args, size_t arg_num, bool& exception_thrown)
            {
                XyA_Function_Check_Arg_Num(1)
                
                return XyA_Allocate(IntObject, sizeof(*args[0]));
            }
        }
    }
}