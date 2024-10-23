export module  Core:ConstantWrapper;

import <llvm/IR/Constant.h>;
import InstructionWrapper;


export struct ConstantWrapper {
	ConstantWrapper(llvm::Constant* constant, unsigned int id, std::shared_ptr<InstructionWrapper> instructionWrapper):constant(constant),
	ID(id),
	instructionWrapper(instructionWrapper)
	{
	}
	~ConstantWrapper()
	{

	}

	llvm::Constant* constant = nullptr;
	unsigned int ID = 0;
	std::shared_ptr<InstructionWrapper> instructionWrapper;
};
