#ifndef CONSTANTWRAPPER_H
#define CONSTANTWRAPPER_H

#include "InstructionWrapper.h"

#include <llvm/IR/Constant.h>

struct ConstantWrapper {
	ConstantWrapper(llvm::Constant* constant, unsigned int id, std::shared_ptr<InstructionWrapper> instructionWrapper);
	~ConstantWrapper();


	llvm::Constant* constant = nullptr;
	unsigned int ID = 0;
	std::shared_ptr<InstructionWrapper> instructionWrapper;
};

#endif // ! 
