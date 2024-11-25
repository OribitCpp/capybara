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

	static int getOperandNum(llvm::Value* value, std::unordered_map<llvm::Instruction*, uint32_t>& registerMap
		, std::shared_ptr<ModuleWrapper> moduleWraper
		, std::shared_ptr<InstructionWrapper> instructionWrapper);
	static llvm::Function* getTargetFunction(llvm::Value* calledVal);
	uint32_t getArgRegister(uint32_t index) { return index; }
public:
	llvm::Function* function;
	uint32_t numArgs;
	uint32_t numRegisters;
	uint32_t numInstructions;
	bool trackCoverage;
	std::vector<std::shared_ptr<InstructionWrapper>> instructions;
	std::unordered_map<llvm::BasicBlock*, uint32_t> basicBlockMap;
};

#endif // ! FUNCTION?_H
