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

            /* 
                参数：所导入的模块(Module)在当前CodeObject的
                CodeObject::module_datas中的index
            */
            ImportModule,
        };

        class Instruction
        {
        public:
            size_t parameter;
            InstructionType type;

            Instruction();
            Instruction(InstructionType type);
            std::string to_string() const;
        };
    }
}