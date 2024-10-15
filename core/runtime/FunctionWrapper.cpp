#include "FunctionWrapper.h"

using namespace llvm;

FunctionWrapper::FunctionWrapper(Function* func, std::shared_ptr<ModuleWrapper> moduleWrapper):
	function(func),
	numArgs(func->arg_size()),
	numInstructions(0),
	numRegisters(0),
	trackCoverage(true)
{
	for (BasicBlock &basicBlock : *function) {
		basicBlockMap[&basicBlock] = numInstructions;
		numInstructions += basicBlock.size();
	}

	instructions.resize(numInstructions);

	std::unordered_map<llvm::Instruction*, unsigned int> registerMap;
	unsigned int rnum = numArgs;
	for (auto bbIter = function->begin(); bbIter != function->end(); ++bbIter) {
		for (auto iter = bbIter->begin(); iter != bbIter->end(); ++iter) {
			registerMap[&*iter] = rnum++;
		}
	}

	numRegisters = rnum;

	unsigned int index = 0;
	for (auto bbIter = function->begin(); bbIter != function->end(); ++bbIter) {
		for (auto iter = bbIter->begin(); iter != bbIter->end(); ++iter) {
			std::shared_ptr<InstructionWrapper> instPtr = std::make_shared<InstructionWrapper>();
			instPtr->instruction = &*iter;
			instPtr->dest = registerMap[&*iter];

			if (isa<CallInst>(iter) || isa<InvokeInst>(iter)) {
				const CallBase& callBase = cast<CallBase>(*iter);
				Value* val = callBase.getCalledOperand();
				instPtr->operands.resize(callBase.arg_size() + 1);
				instPtr->operands[0] = getOperandNum(val, registerMap, moduleWrapper,instPtr);
				for (unsigned int number = 0; number < numArgs; number++) {
					Value* value = callBase.getArgOperand(number);
					instPtr->operands[number + 1] = getOperandNum(value, registerMap, moduleWrapper, instPtr);
				}
			}
			else {
				unsigned int numOperands = iter->getNumOperands();
				instPtr->operands.resize(numOperands);
				for (unsigned int j = 0; j < numOperands; j++) {
					Value* value = iter->getOperand(j);
					instPtr->operands[j] = getOperandNum(value, registerMap, moduleWrapper, instPtr);
				}
			}

			instructions[index++] = instPtr;
		}
	}

}

FunctionWrapper::~FunctionWrapper()
{
}

llvm::StringRef FunctionWrapper::getName() const
{
	return function->getName();
}

llvm::FunctionType* FunctionWrapper::getType() const
{
	return function->getFunctionType();
}

int FunctionWrapper::getOperandNum(Value* value, std::unordered_map<Instruction*, unsigned int>& registerMap, std::shared_ptr<ModuleWrapper> moduleWraper, std::shared_ptr<InstructionWrapper> instructionWrapper)
{
	if (Instruction* inst = dyn_cast<Instruction>(value)) {
		return registerMap[inst];
	}
	else if (Argument* arg = dyn_cast<Argument>(value)) {
		return arg->getArgNo();
	}else if(isa<BasicBlock>(value) || isa<InlineAsm>(value) || isa<MetadataAsValue>(value)) {
		return -1;
	}
	else {
		assert(isa<Constant>(value));
		Constant* constant = cast<Constant>(value);
		return -(moduleWraper->getConstantID(constant, instructionWrapper) + 2);
	}
}
