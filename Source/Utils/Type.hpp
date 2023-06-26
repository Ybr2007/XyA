namespace XyA
{
    namespace Utils
    {
        template <typename T, typename T_Arg>
        bool is_instance(T_Arg obj)
        {
            return dynamic_cast<T>(obj) == nullptr;
        }
    }
}