export module FunctionWrapper;

import <memory>;
import <llvm/IR/Instruction.h>;
import <llvm/IR/Instructions.h>;
import InstructionWrapper;
import ModuleWrapper;


export struct FunctionWrapper {
	FunctionWrapper(llvm::Function* func, std::shared_ptr<ModuleWrapper> moduleWrapper);

	llvm::StringRef getName() const;
	llvm::FunctionType* getType() const;

	int getOperandNum(llvm::Value* value, std::unordered_map<llvm::Instruction*, unsigned int>& registerMap, std::shared_ptr<ModuleWrapper> moduleWraper, std::shared_ptr<InstructionWrapper> instructionWrapper);
public:
	llvm::Function* function;
	unsigned int numArgs;
	unsigned int numRegisters;
	unsigned int numInstructions;
	bool trackCoverage;
	std::vector<std::shared_ptr<InstructionWrapper>> instructions;
	std::unordered_map<llvm::BasicBlock*, unsigned int> basicBlockMap;
};