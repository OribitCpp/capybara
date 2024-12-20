#include "FunctionWrapper.h"
#include <llvm/IR/Constants.h>
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

int FunctionWrapper::getOperandNum(Value* value, std::unordered_map<Instruction*, uint32_t>& registerMap, std::shared_ptr<ModuleWrapper> moduleWraper, std::shared_ptr<InstructionWrapper> instructionWrapper)
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

llvm::Function* FunctionWrapper::getTargetFunction(llvm::Value* calledVal)
{
	SmallPtrSet<const GlobalValue*, 3> Visited;

	Constant* c = dyn_cast<Constant>(calledVal);
	if (!c)	return nullptr;

	while (true) {
		if (GlobalValue* gv = dyn_cast<GlobalValue>(c)) {
			if (!Visited.insert(gv).second)	return nullptr;

			if (Function* f = dyn_cast<Function>(gv))	return f;
			else if (GlobalAlias* ga = dyn_cast<GlobalAlias>(gv))	c = ga->getAliasee();
			else	return nullptr;
		}
		else if (llvm::ConstantExpr* ce = dyn_cast<llvm::ConstantExpr>(c)) {
			if (ce->getOpcode() == Instruction::BitCast)
				c = ce->getOperand(0);
			else	return nullptr;
		}
		else	return nullptr;
	}
}

llvm::Function* FunctionWrapper::getDirectCallTarget(const llvm::CallBase& cb, bool moduleIsFullyLinked)
{
	Value* v = cb.getCalledOperand();
	bool viaConstantExpr = false;
	// Walk through aliases and bitcasts to try to find
	// the function being called.
	do {
		if (isa<llvm::GlobalVariable>(v)) {
			// We don't care how we got this GlobalVariable
			viaConstantExpr = false;

			// Global variables won't be a direct call target. Instead, their
			// value need to be read and is handled as indirect call target.
			v = nullptr;
		}
		else if (Function* f = dyn_cast<Function>(v)) {
			return f;
		}
		else if (llvm::GlobalAlias* ga = dyn_cast<GlobalAlias>(v)) {
			if (moduleIsFullyLinked || !(ga->isInterposable())) {
				v = ga->getAliasee();
			}
			else {
				v = nullptr;
			}
		}
		else if (llvm::ConstantExpr* ce = dyn_cast<llvm::ConstantExpr>(v)) {
			viaConstantExpr = true;
			v = ce->getOperand(0)->stripPointerCasts();
		}
		else {
			v = nullptr;
		}
	} while (v != nullptr);

	// NOTE: This assert may fire, it isn't necessarily a problem and
	// can be disabled, I just wanted to know when and if it happened.
	(void)viaConstantExpr;
	assert((!viaConstantExpr) &&
		"FIXME: Unresolved direct target for a constant expression");
	return nullptr;
}
