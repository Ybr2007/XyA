#pragma once
#include <functional>
#include <TypeDefs.h>
#include <LexicalAnalysis/Token.h>


namespace XyA
{

    enum class MessageLevel
    {
        Note,
        Warning,
        Error
    };

    struct Message
    {
        StringView content;
        MessageLevel level;

        Message(StringView message, MessageLevel level = MessageLevel::Note);
    };

    using MessageCallback = std::function<void(Message, LexicalAnalysis::TokenPos)>;
       
} // namespace XyA
