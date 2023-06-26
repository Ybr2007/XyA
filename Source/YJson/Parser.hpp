#pragma once
#include <YJson/Exception.hpp>
#include <YJson/Object.hpp>


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

    /*
    解析json文本

    Args:
        std::string_view jsonString -> json文本

    Return:
        YJson::Object* -> 解析得到的对象
    */
    Object* Parser::parse(std::string_view jsonString)
    {
        this->__jsonString = jsonString;
        this->__curPos = 0;
        this->__finished = false;

        if (this->__jsonString.empty())
        {
            throw ParsingException("Empty string: The json string is empty.", 0);
        }

        Object* result = this->__parseObject();

        if (!this->__finished)
        {
            while (true)
            {
                if (!this->__isBlank(this->__curChar()))
                {
                    throw ParsingException("Multiple Root Objects: A JSON should only have one root object.", this->__curPos);
                }
                if (!this->__tryMovePtr())
                {
                    break;
                }
            }
        }

        return result;
    }

    Object* Parser::__parseObject()
    {
        // 跳过先导空白
        while (this->__isBlank(this->__curChar()))
        {
            if(!__tryMovePtr())  // 从此处到结尾全是空白
            {
                throw ParsingException("Empty String: The remaining chars of the json string are all blank", this->__curPos);
            }
        }

        switch (this->__curChar())
        {
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return this->__parseNumber();
        case '"': 
            return this->__parseString();
        case 't': 
            return this->__parseTrue();
        case 'f': 
            return this->__parseFalse(); 
        case 'n': 
            return this->__parseNull();
        case '{': 
            return this->__parseDict();
        case '[': 
            return this->__parseList();

        default: throw ParsingException("Unknown object.", this->__curPos);
        }
    }

    Object* Parser::__parseNull()
    {
        if (this->__match("null", 4))
        {
            return new Object();
        }
        else
        {
            throw ParsingException("Try to parse an unknown object as null.", this->__curPos);
        }
    }

    Object* Parser::__parseNumber()
    {
        Pos numberBeginPos = this->__curPos;  // 数字第一个字符的位置
        Pos numberEndPos;  // 数字最后一位的位置
        
        bool foundDecimalDot = false;
        bool foundExponentSymbol = false;
        bool foundMinusSign = false;
        bool foundZeroAtBegin = false;
        Pos beginningZeroPos;
        Pos decimalDotPos;
        Pos exponentSymbolPos;

        while (true)
        {
            if (this->__isExponentSymbol(this->__curChar()))
            {
                if ((foundMinusSign && this->__curPos == numberBeginPos + 1) || (!foundMinusSign && this->__curPos == numberBeginPos))
                {
                    throw ParsingException("Illegal number: Found an exponent symbol following the minus sign.", this->__curPos);
                }
                if (!foundExponentSymbol)  // 检查是否已经存在'e'
                {
                    foundExponentSymbol = true;
                    exponentSymbolPos = this->__curPos;
                }
                else
                {
                    throw ParsingException("Illegal number: Multiple exponent symbols found.", this->__curPos);
                }
            }
            else if (this->__curChar() == '.')
            {
                if (foundMinusSign && this->__curPos == numberBeginPos + 1)
                {
                    throw ParsingException("Illegal number: Found a decimal dot following the minus sign.", this->__curPos);
                }
                if (!foundDecimalDot && !foundExponentSymbol)  // 只能有一个小数点，并且小数点不能在指数中
                {
                    foundDecimalDot = true;
                    decimalDotPos = this->__curPos;
                }
                else
                {
                    throw ParsingException("Illegal number: Found a decimal dot after the first dot or exponent symbol.", this->__curPos);
                }
            } 
            else if (this->__curChar() == '-')
            {
                // 如果负号不在数字的开头或指数的开头则属于非法数字
                if (this->__curPos != numberBeginPos && !(foundExponentSymbol && this->__curPos == exponentSymbolPos + 1))
                {
                    throw ParsingException("Illegal number: The minus sign must be at the beginning of the number or the exponent.", this->__curPos);
                }
                foundMinusSign = true;
            }
            else if (!this->__isDigit(__curChar()))
            {
                numberEndPos = this->__curPos - 1;  // 如果这一位不是数字/负号/e/小数，那么数字的最后一位应该是当前位的前一位
                break;
            }
            else
            {
                if (this->__curChar() == '0')
                {
                    if ((foundMinusSign && this->__curPos == numberBeginPos + 1) || 
                        (!foundMinusSign && this->__curPos == numberBeginPos))
                    {
                        foundZeroAtBegin = true;
                        beginningZeroPos = this->__curPos;
                    }
                }
            }

            if (!this->__tryMovePtr())  // 如果这一位是合法的，但是是json的最后一个字符，那么数字的最后一位应该是当前位
            {
                numberEndPos = this->__curPos;
                break;
            }
        }

        if (foundZeroAtBegin && beginningZeroPos != numberEndPos && !(foundDecimalDot && beginningZeroPos == decimalDotPos - 1))  // 先导0
        {
            throw ParsingException("Illegal number: Found a zero at beginning.", beginningZeroPos);
        }

       if (this->__isExponentSymbol(this->__jsonString[numberEndPos]) || this->__jsonString[numberEndPos] == '.' || 
            this->__jsonString[numberEndPos] == '-')
        {
            throw ParsingException("Illegal number: 'e', '-' or '.' can not be the end of the number.", numberEndPos);
        }

        Number number = std::stod((String)this->__jsonString.substr(numberBeginPos, numberEndPos - numberBeginPos + 1));
        return new Object(number);
    }

    Object* Parser::__parseString()
    {
        if (!this->__tryMovePtr())  // 从左引号的下一个字符开始
        {
            throw ParsingException("Right quotation mark not found.", this->__curPos);
        }

        Pos firstCharPos = this->__curPos;
        bool escape = false;

        String result = "";

        while (this->__curChar() != '"' || escape)
        {  
            if (this->__curChar() == '\\')
            {
                escape = true;
            }
            else
            {
                result.push_back(this->__curChar());
            }

            if (this->__curChar() != '\\' && escape)
            {
                escape = false;
            }

            if (!this->__tryMovePtr())
            {
                throw ParsingException("Right quotation mark not found.", this->__curPos);
            }
        }

        this->__tryMovePtr();  // 跳过右引号

        return new Object(result);
    }

    Object* Parser::__parseTrue()
    {
        if (this->__match("true", 4))
        {
            return new Object(true);
        }
        else
        {
            throw ParsingException("Try to parse an unknown object as boolean.", this->__curPos);
        }
    }

    Object* Parser::__parseFalse()
    {
        if (this->__match("false", 5))
        {
            return new Object(false);
        }
        else
        {
            throw ParsingException("Try to parse an unknown object as boolean.", this->__curPos);
        }
    }

    Object* Parser::__parseList()
    {
        if (!this->__tryMovePtr())  // 从左中括号的下一个字符开始
        {
            throw ParsingException("Right middle bracket not found.", this->__curPos);
        }

        List list;
        bool foundRightMiddleBracket = false;

        if (this->__curChar() == ']')
        {
            this->__tryMovePtr();
            return new Object(list);
        }

        while (true)
        {
            Object* item = this->__parseObject();
            list.push_back(item);

            if (this->__finished)
            {
                throw ParsingException("Right middle bracket not found.", this->__curPos);
            }

            bool foundComma = false;
            while (true)
            {
                if (this->__curChar() == ',')
                {
                    if (!foundComma)
                    {
                        foundComma = true;
                    }
                    else
                    {
                        throw ParsingException("Item not found.", this->__curPos);
                    }
                }
                else if (!this->__isBlank(this->__curChar()))
                {
                    if (this->__curChar() == ']')
                    {
                        foundRightMiddleBracket = true;
                        this->__tryMovePtr();
                    }
                    else if (!foundComma)
                    {
                        throw ParsingException("Comma not found.", this->__curPos);
                    }
                    break;
                }

                if (!this->__tryMovePtr())
                {
                    throw ParsingException("Right middle bracket not found.", this->__curPos);
                }
            }

            if (foundRightMiddleBracket)
            {
                if (!foundComma)
                {
                    break;
                }
                throw ParsingException("A comma at the end of the list.", this->__curPos);
            }
        }

        return new Object(list);
    }

    Object* Parser::__parseDict()
    {
        if (!this->__tryMovePtr())  // 从左大括号的下一个字符开始
        {
            throw ParsingException("Right brace not found.", this->__curPos);
        }

        Dict dict;
        bool foundRightBrace = false;

        if (this->__curChar() == '}')  // 空dict
        {
            this->__tryMovePtr();
            return new Object(dict);
        }

        while (true)
        {
            /* 解析key */
            Object* key = this->__parseObject();
            if (key->type() != ObjectType::String)
            {
                throw ParsingException("Type of the key is not the string.", this->__curPos);
            }

            /* 解析冒号 */
            bool foundColon = false;
            while (true)
            {
                if (this->__curChar() == ':')
                {
                    this->__tryMovePtr();  // 跳过冒号
                    break;
                }
                else if (!this->__isBlank(this->__curChar()))
                {
                    throw ParsingException("An illegal char before the colon.", this->__curPos);
                }

                if (!this->__tryMovePtr())
                {
                    throw ParsingException("Colon not found.", this->__curPos);
                }
            }
            if (this->__finished)  // 如果冒号是最后一个字符
            {
                throw ParsingException("Value not found.", this->__curPos);
            }

            while (this->__isBlank(this->__curChar()))
            {
                if (!this->__tryMovePtr())
                {
                    throw ParsingException("Brace not found.", this->__curPos);
                }
            }

            if (this->__curChar() == '}') 
            {
                throw ParsingException("Value not found.", this->__curPos);
            }

            /* 解析value */
            Object* value = this->__parseObject();
            dict[key->as<String>()] = value;
            delete key;

            if (this->__finished)
            {
                throw ParsingException("Right brace not found.", this->__curPos);
            }

            /* 解析逗号 */
            bool foundComma = false;
            while (true)
            {
                if (this->__curChar() == ',')
                {
                    if (!foundComma)
                    {
                        foundComma = true;
                    }
                    else
                    {
                        throw ParsingException("Key not found.", this->__curPos);
                    }
                }
                else if (!this->__isBlank(this->__curChar()))
                {
                    if (this->__curChar() == '}')
                    {
                        foundRightBrace = true;
                        this->__tryMovePtr();
                    }
                    else if (!foundComma)
                    {
                        throw ParsingException("Comma not found.", this->__curPos);
                    }
                    break;
                }

                if (!this->__tryMovePtr())
                {
                    throw ParsingException("Right brace not found.", this->__curPos);
                }
            }

            if (foundRightBrace)
            {
                if (!foundComma)
                {
                    break;
                }
                throw ParsingException("Comma at the end of the dict.", this->__curPos);
            }
        }

        return new Object(dict);
    }
    

    char Parser::__curChar()
    {
        return this->__jsonString[this->__curPos];
    }

    bool Parser::__tryMovePtr()
    {
        if (this->__curPos < this->__jsonString.size() - 1)
        {
            this->__curPos ++;
            return true;
        }
        this->__finished = true;
        return false;
    }


    bool Parser::__isBlank(char chr)
    {
        return chr == ' ' || chr == '\n';
    }

    bool Parser::__isExponentSymbol(char chr)
    {
        return chr == 'e' || chr == 'E';
    }

    bool Parser::__isDigit(char chr)
    {
        return chr == '0' || chr == '1' || chr == '2' || chr == '3' || chr == '4' || chr == '5' || chr == '6' || 
            chr == '7' || chr == '8' || chr == '9';
    }

    bool Parser::__match(const char* pattern, size_t patternLength)
    {
        if (this->__curPos + patternLength >= this->__jsonString.size()) return false;
        for (Pos i = 0; i < patternLength; i ++)
        {
            if (pattern[i] != this->__curChar()) return false;
            if (!this->__tryMovePtr()) return false;
        }
        return true;
    }
}
