#pragma once
#include <Runtime/Instruction.h>


namespace XyA
{
    namespace Runtime
    {
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

            case InstructionType::Return:
                str += "Return";
                break;

            case InstructionType::LoadLiteral:
                str += "Load Literal";
                str += " " + std::to_string(this->parameter);
                break;

            case InstructionType::LoadVariable:
                str += "Load Variable";
                str += " " + std::to_string(this->parameter);
                break;
            
            case InstructionType::LoadGlobal:
                str += "Load Global";
                str += " " + std::to_string(this->parameter);
                break;

            case InstructionType::StroeVariable:
                str += "Stroe Variable";
                str += " " + std::to_string(this->parameter);
                break;

            case InstructionType::StoreAttr:
                str += "Stroe Attr";
                str += " " + std::to_string(this->parameter);
                break;

            case InstructionType::PopJumpIfFalse:
                str += "Pop Jump If False";
                str += " " + std::to_string(this->parameter);
                break;

            case InstructionType::JumpTo:
                str += "Jump To";
                str += " " + std::to_string(this->parameter);
                break;

            case InstructionType::CallFunction:
                str += "Call Function";
                str += " " + std::to_string(this->parameter);
                break;

            case InstructionType::GetAttr:
                str += "Get Attr";
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