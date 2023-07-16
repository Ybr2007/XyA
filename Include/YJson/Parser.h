#pragma once
#include <YJson/Exception.hpp>
#include <YJson/Object.h>


namespace YJson
{
    typedef size_t Pos;

    class Parser
    {
    public:
        Object* parse(std::string_view jsonString);

    private:
        Pos __curPos;
        std::string_view __jsonString;
        bool __finished;

        Object* __parseObject();
        Object* __parseNull();
        Object* __parseNumber();
        Object* __parseString();
        Object* __parseTrue();
        Object* __parseFalse();
        Object* __parseList();
        Object* __parseDict();

        char __curChar();
        bool __tryMovePtr();

        bool __isBlank(char chr);
        bool __isExponentSymbol(char chr);
        bool __isDigit(char chr);
        bool __match(const char* pattern, size_t patternLength);
    };
}
