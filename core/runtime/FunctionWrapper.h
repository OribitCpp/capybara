#ifndef  FUNCTIONWRAPPER_H
#define FUNCTIONWRAPPER_H

#include "ModuleWrapper.h"
#include "InstructionWrapper.h"

struct FunctionWrapper {
	FunctionWrapper(llvm::Function *func, std::shared_ptr<ModuleWrapper> moduleWrapper);
	~FunctionWrapper();

	FunctionWrapper(const FunctionWrapper&) = delete;
	FunctionWrapper& operator=(const FunctionWrapper&) = delete;

	llvm::StringRef getName() const;
	llvm::FunctionType *getType() const;

	static int getOperandNum(llvm::Value* value, std::unordered_map<llvm::Instruction*, unsigned int>& registerMap
		, std::shared_ptr<ModuleWrapper> moduleWraper
		, std::shared_ptr<InstructionWrapper> instructionWrapper);
public:
	llvm::Function* function;
	unsigned int numArgs;
	unsigned int numRegisters;
	unsigned int numInstructions;
	bool trackCoverage;
	std::vector<std::shared_ptr<InstructionWrapper>> instructions;
	std::unordered_map<llvm::BasicBlock*, unsigned int> basicBlockMap;
};

#endif // ! FUNCTION?_H
