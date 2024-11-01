export module  StackFrame;

import <vector>;
import <memory>;
import Expr;

import InstructionWrapper;
import FunctionWrapper;

export class StackFrame {
public:
	StackFrame(std::vector<std::shared_ptr<InstructionWrapper>>::iterator caller, std::shared_ptr<FunctionWrapper> func) :
	caller(caller),
	function(func)
	{
		locals.resize(function->numRegisters);
	}

	StackFrame(const StackFrame& other)
	{
		caller = other.caller;
		function = other.function;
		locals = other.locals;
	}

	~StackFrame()
	{
	}


public:
	std::vector<std::shared_ptr<Expr>> locals;
	std::shared_ptr<FunctionWrapper> function;
	std::vector<std::shared_ptr<InstructionWrapper>>::iterator caller;
};
