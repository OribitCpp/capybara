#include "ConstantWrapper.h"

ConstantWrapper::ConstantWrapper(llvm::Constant* constant, unsigned int id, InstructionWrapper* instructionWrapper):
	constant(constant),
	ID(id),
	instructionWrapper(instructionWrapper)
{
}

ConstantWrapper::~ConstantWrapper()
{
}
