#ifndef  EXECUTIONSTATE_H
#define EXECUTIONSTATE_H

#include "FunctionWrapper.h"
#include "StackFrame.h"
#include <memory>
#include <vector>

class ExecutionState  {
public:
	ExecutionState(std::shared_ptr<FunctionWrapper> &func);
	ExecutionState& operator=(const ExecutionState&) = delete;
	ExecutionState(ExecutionState&&) = delete;
	ExecutionState& operator=(ExecutionState&&) = delete;

	void pushFrame(std::vector<std::shared_ptr<InstructionWrapper>>::iterator pc, std::shared_ptr<FunctionWrapper> func);
	void popFrame();
public:
	std::vector<std::shared_ptr<InstructionWrapper>>::iterator PC;
	std::vector<std::shared_ptr<InstructionWrapper>>::iterator prevPC;
	
private:
	void setID();
private:
	std::vector<StackFrame> m_stack;
	static unsigned int s_total;
	unsigned int ID;
};

#endif // ! EXECUTIONSTATE_H
