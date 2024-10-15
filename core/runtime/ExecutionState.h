#ifndef  EXECUTIONSTATE_H
#define EXECUTIONSTATE_H

#include "FunctionWrapper.h"
#include "MemoryManager.h"
#include "StackFrame.h"
#include <memory>
#include <vector>

class ExecutionState  {
public:
	ExecutionState(std::shared_ptr<FunctionWrapper> &func, std::shared_ptr<MemoryManager> &memoryManager);
	void pushFrame(std::vector<std::shared_ptr<InstructionWrapper>>::iterator pc, std::shared_ptr<FunctionWrapper> func);
public:
	std::vector<std::shared_ptr<InstructionWrapper>>::iterator PC;
	std::vector<std::shared_ptr<InstructionWrapper>>::iterator prevPC;
	
private:

private:
	std::vector<StackFrame> m_stack;

};

#endif // ! EXECUTIONSTATE_H
