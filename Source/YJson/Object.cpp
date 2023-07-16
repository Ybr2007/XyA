#include <YJson/Object.h>


namespace YJson
{    
    std::ostream& operator<<(std::ostream& os, const Object& self)
    {
        os << self.toString();
        return os;
    }
}