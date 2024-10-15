#include "ConstantWrapper.h"

ConstantWrapper::ConstantWrapper(llvm::Constant* constant, unsigned int id, std::shared_ptr<InstructionWrapper> instructionWrapper):
	constant(constant),
	ID(id),
	instructionWrapper(instructionWrapper)
{
}

ConstantWrapper::~ConstantWrapper()
{
}
