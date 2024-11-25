#include "InstructionWrapper.h"

std::string InstructionWrapper::sourceLocation() const
{
    std::string result = "[no debug info]";
    if (!file.empty()) {
        result = file + ": " + std::to_string(line) + " " + std::to_string(column);
    }
    return  result;
}

uint32_t InstructionWrapper::getOpcode() const
{
    return instruction->getOpcode();
}
