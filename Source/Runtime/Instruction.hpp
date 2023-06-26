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

            /* 有参数 */
            LoadLiteral,
            LoadVariable,

            StroeVariable,

            PopJumpIfFalse,
            JumpForward,


            CallFunction,
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

        Instruction::Instruction()
        {
            
        }

        Instruction::Instruction(InstructionType type)
        {
            this->type = type;
        }

        std::string Instruction::to_string() const
        {
            std::string str = "<";
            switch (this->type)
            {
            case InstructionType::BinaryAdd:
                str += "Binary Add";
                break;

            case InstructionType::BinarySubtract:
                str += "Binary Subtract";
                break;

            case InstructionType::BinaryMultiply:
                str += "Binary Multiply";
                break;

            case InstructionType::BinaryDevide:
                str += "Binary Devide";
                break;

            case InstructionType::CompareIfEqual:
                str += "Compare If Equal";
                break;

            case InstructionType::CompareIfNotEqual:
                str += "Compare If Not Equal";
                break;

            case InstructionType::CompareIfGreaterThan:
                str += "Compare If Greater Than";
                break;

            case InstructionType::CompareIfGreaterEqual:
                str += "Compare If Greater Equal";
                break;

            case InstructionType::CompareIfLessThan:
                str += "Compare If Less Than";
                break;

            case InstructionType::CompareIfLessEqual:
                str += "Compare If Less Equal";
                break;

            case InstructionType::UnaryNegative:
                str += "Unary Negative";
                break;

             case InstructionType::UnaryNot:
                str += "Unary Not";
                break;

            case InstructionType::PopTop:
                str += "Pop Top";
                break;

            case InstructionType::LoadLiteral:
                str += "Load Literal";
                str += " " + std::to_string(this->parameter);
                break;

            case InstructionType::LoadVariable:
                str += "Load Variable";
                str += " " + std::to_string(this->parameter);
                break;

            case InstructionType::StroeVariable:
                str += "Stroe Variable";
                str += " " + std::to_string(this->parameter);
                break;

            case InstructionType::PopJumpIfFalse:
                str += "Pop Jump If False";
                str += " " + std::to_string(this->parameter);
                break;

            case InstructionType::JumpForward:
                str += "Jump Forward";
                str += " " + std::to_string(this->parameter);
                break;

            case InstructionType::CallFunction:
                str += "Call Function";
                str += " " + std::to_string(this->parameter);
                break;

                
            
            default:
                break;
            }
            str += ">";
            return str;
        }
    }
}