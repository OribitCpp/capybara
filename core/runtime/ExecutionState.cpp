#include "ExecutionState.h"
#include "memory/MemoryManager.h"

unsigned int ExecutionState::s_total = 0;

ExecutionState::ExecutionState(std::shared_ptr<FunctionWrapper>& func):
	PC(func->instructions.begin()),
	prevPC(PC)
{
	pushFrame(func->instructions.end(), func);
	setID();

}

void ExecutionState::pushFrame(std::vector<std::shared_ptr<InstructionWrapper>>::iterator pc, std::shared_ptr<FunctionWrapper> func)
{
	m_stack.emplace_back(pc, func);
}

void ExecutionState::popFrame()
{
	m_stack.pop_back();
}

void ExecutionState::setID()
{
	ID = ++s_total;
}
