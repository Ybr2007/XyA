#pragma once
#include <utility>


namespace XyA
{

    template <typename T>
    class TemporarySetter 
    {
    public:
        TemporarySetter(T& variable, T value) : __variable(variable)
        {
            this->__originalValue = variable;
            variable = value;
        }

        ~TemporarySetter() 
        {
            this->__variable = this->__originalValue;
        }

    private:
        T& __variable;
        T __originalValue;
    };

}