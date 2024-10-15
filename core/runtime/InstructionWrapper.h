#ifndef INSTRUCTIONWRAPPER_H
#define INSTRUCTIONWRAPPER_H

#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <vector>

struct InstructionWrapper {
	llvm::Instruction* instruction = nullptr;
	unsigned int dest = 0;
	uint64_t offset = 0;
	std::vector<int> operands;
	std::vector<std::pair<unsigned int, uint64_t>> indices;
};

#endif