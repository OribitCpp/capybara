#include "StackFrame.h"

StackFrame::StackFrame(std::vector<std::shared_ptr<InstructionWrapper>>::iterator caller, std::shared_ptr<FunctionWrapper> func) :
	caller(caller),
	function(func)
{
	locals.resize(function->numRegisters);
}

StackFrame::StackFrame(const StackFrame& other)
{
}

StackFrame::~StackFrame()
{
}
