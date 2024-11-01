export module InstructionWrapper;

import <llvm/IR/Instruction.h>;
import <llvm/IR/Instructions.h>;
import <vector>;

export class InstructionWrapper {
public:
	llvm::Instruction* instruction = nullptr;
	unsigned int dest = 0;
	uint64_t offset = 0;
	std::vector<int> operands;
	std::vector<std::pair<unsigned int, uint64_t>> indices;
};