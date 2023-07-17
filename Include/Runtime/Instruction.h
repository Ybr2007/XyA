#pragma once
#include <string>


namespace XyA
{
    namespace Runtime
    {
        enum class InstructionType
        {    
            /* 无参数 */
            BinaryAdd,
            BinarySubtract,
            BinaryMultiply,
            BinaryDevide,

            CompareIfEqual,
            CompareIfNotEqual,
            CompareIfGreaterThan,
            CompareIfGreaterEqual,
            CompareIfLessThan,
            CompareIfLessEqual,

            UnaryNegative,
            UnaryNot,

            PopTop,
            Return,

            ConvertType,

            /* 有参数 */
            LoadLiteral,
            LoadVariable,
            LoadGlobal,

            StroeVariable,
            StorePublicAttr,
            StroePrivateAttr,

            PopJumpIfFalse,
            JumpTo,

            CallFunction,
            CallMethod,

            GetAttr,
            GetMethod,
        };

        class Instruction
        {
        public:
            InstructionType type;
            size_t parameter = 0;

            Instruction();
            Instruction(InstructionType type);
            std::string to_string() const;
        };
    }
}