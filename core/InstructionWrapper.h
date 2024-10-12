#ifndef INSTRUCTIONWRAPPER_H
#define INSTRUCTIONWRAPPER_H

#include <llvm/IR/Instruction.h>
#include <vector>

struct InstructionWrapper {
	llvm::Instruction* instruction;
	unsigned int dest;
	std::vector<int> operands;
};

#endif