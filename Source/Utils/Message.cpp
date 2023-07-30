#include <Utils/Message.h>


namespace XyA
{
    Message::Message(StringView message, MessageLevel level)
    {
        this->content = message;
        this->level = level;
    }
}