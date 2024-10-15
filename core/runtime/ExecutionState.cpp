#include "ExecutionState.h"

ExecutionState::ExecutionState(std::shared_ptr<FunctionWrapper>& func, std::shared_ptr<MemoryManager>& memoryManager):
	PC(func->instructions.begin()),
	prevPC(PC)
{
}

void ExecutionState::pushFrame(std::vector<std::shared_ptr<InstructionWrapper>>::iterator pc, std::shared_ptr<FunctionWrapper> func)
{
}
