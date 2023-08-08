#pragma once
#include <utility>


namespace XyA
{

    template <typename T>
    class TemporarySetter 
    {
    public:
        TemporarySetter(T& variable_ref, T value) : __variable_ref(variable_ref)
        {
            this->__originalValue = variable_ref;
            variable_ref = value;
        }

        TemporarySetter(const TemporarySetter&) = delete;
        TemporarySetter(TemporarySetter&&) = delete;
        TemporarySetter& operator=(const TemporarySetter&) = delete;

        ~TemporarySetter() 
        {
            this->__variable_ref = this->__originalValue;
        }

    private:
        T& __variable_ref;
        T __originalValue;
    };

}