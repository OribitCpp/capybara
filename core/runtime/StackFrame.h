#ifndef  STACKFRAME_H
#define STACKFRAME_H

#include "InstructionWrapper.h"
#include "FunctionWrapper.h"
#include "expr/Expr.h"

class StackFrame {
public:
	StackFrame(std::vector<std::shared_ptr<InstructionWrapper>>::iterator caller, std::shared_ptr<FunctionWrapper> func);
	StackFrame(const StackFrame& other);
	~StackFrame();

public:
	std::vector<std::shared_ptr<Expr>> locals;
	std::shared_ptr<FunctionWrapper> function;
	std::vector<std::shared_ptr<InstructionWrapper>>::iterator caller;
};

#endif // ! STACKFRAME_H
