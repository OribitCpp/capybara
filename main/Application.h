#ifndef  APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <unordered_map>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>

#include "Logger.h"
#include "expr/ConstantExpr.h"
#include "runtime/ModuleWrapper.h"
#include "runtime/ExecutionState.h"
#include "runtime/ExternalDispatcher.h"
#include "runtime/TerminationType.h"
#include "runtime/ObjectState.h"
#include "runtime/ExprOptimizer.h"

#include "SolverManager.h"

class Application {
public:
	Application(int argc, char* argv[]);
	void init();
	void ready();
	void execute();
	void finish(){}
	void exit();

private:
	void parseConsoleParameters(int argc, char* argv[]);

	//logic
	void initializeGlobals(std::shared_ptr<ExecutionState>& state);
	void allocateGlobalObjects(std::shared_ptr<ExecutionState>& state);
	void initializeGlobalsAllias(const llvm::Constant *constant);
	void initializeGlobalObjects(std::shared_ptr<ExecutionState>& state);

	std::shared_ptr<ConstantExpr> evalConstant(const llvm::Constant* c, std::shared_ptr<InstructionWrapper> instruction = nullptr);
	std::shared_ptr<ConstantExpr> evalConstantExpr(const llvm::ConstantExpr* expr, const std::shared_ptr<InstructionWrapper>& instruction);

	void branch(std::shared_ptr<ExecutionState>& state, const std::vector<std::shared_ptr<Expr>>& conditions, std::vector<std::shared_ptr<ExecutionState>>& result, BranchType reason);
	void executeInstructon(std::shared_ptr<ExecutionState>& state, std::shared_ptr<InstructionWrapper>& instruction);
	void terminateStateOnExit(std::shared_ptr<ExecutionState>& state);
	void terminateStateOnExecError(std::shared_ptr<ExecutionState>& state, const std::string& message, StateTerminationType reason = StateTerminationType::Execution);
	void terminateStateOnProgramError(std::shared_ptr<ExecutionState>& state, const std::string& message, StateTerminationType reason);
	void terminateStateOnSolverError(std::shared_ptr<ExecutionState>& state, const std::string& message);
	void terminateStateOnError(std::shared_ptr<ExecutionState>& state, const std::string &message, StateTerminationType reason);
	void transferToBasicBlock(llvm::BasicBlock* dst, llvm::BasicBlock* src, std::shared_ptr<ExecutionState>& state);
	std::pair< std::shared_ptr<ExecutionState>, std::shared_ptr<ExecutionState>> fork(std::shared_ptr<ExecutionState>& currentState, const std::shared_ptr<Expr>& condition, bool isInternal, BranchType type);
	std::shared_ptr<Expr> toUnique(std::shared_ptr<ExecutionState>& currentState, std::shared_ptr<Expr> expr);
	std::shared_ptr<ConstantExpr> toConstant(const std::shared_ptr<ExecutionState>& state, std::shared_ptr<Expr> expr, bool concretize = true);
	void executeCall(std::shared_ptr<ExecutionState>& state, const std::shared_ptr<InstructionWrapper>& wrapper, llvm::Function* func, std::vector<std::shared_ptr<Expr>>& arguments);
	void executeAlloc(std::shared_ptr<ExecutionState>& state, std::shared_ptr<Expr> size, bool isLocal, std::shared_ptr<InstructionWrapper> target,
		bool zeroMemory = false, const std::shared_ptr<ObjectState> &reallocFrom = nullptr, size_t allocationAlignment = 0);
	std::shared_ptr<InstructionWrapper> getLastNonKleeInternalInstruction(const std::shared_ptr<ExecutionState>& state, std::shared_ptr<InstructionWrapper> & lastInstruction);
	void executeMemoryOperation(std::shared_ptr<ExecutionState>& state, bool isWrite, std::shared_ptr<Expr> address, std::shared_ptr<Expr> value, std::shared_ptr<InstructionWrapper> target);
	void processTestCase(const std::shared_ptr<ExecutionState>& state, const std::string& message);
	std::shared_ptr<ConstantExpr> getEhTypeidFor(std::shared_ptr<Expr> typeInfo);
	void terminateState(const std::shared_ptr<ExecutionState>& state, StateTerminationType reason);
	void terminateStateOnUserError(std::shared_ptr<ExecutionState>& state, const std::string& message, bool writeErr = true);
	void terminateStateEarly(std::shared_ptr<ExecutionState>& state, const std::string& message,StateTerminationType reason);
	bool shouldExitOn(StateTerminationType reason);
	void callExternalFunction(std::shared_ptr<ExecutionState>& state,std::shared_ptr<InstructionWrapper> target, std::shared_ptr<FunctionWrapper> &callable, std::vector< std::shared_ptr<Expr> >& arguments);
	uint64_t getAllocationAlignment(const llvm::Value* allocSite) const;
private:
	int argc = 0;
	char** argv = nullptr;
	bool m_haltExecution = false;
	std::shared_ptr<llvm::LLVMContext> m_llvmContext;
	std::shared_ptr<ModuleWrapper> m_moduleStorage;
	std::shared_ptr<ExternalDispatcher> m_externalDispatcher;
	std::unordered_map<uint64_t, llvm::Function*> m_legalFunctions;
	std::unordered_map<const llvm::GlobalValue*, std::shared_ptr<ConstantExpr>> m_globalAddresses;
	std::unordered_map<const llvm::GlobalValue*, std::shared_ptr<MemoryObject>> m_globalObjects;
	std::set<std::shared_ptr<ExecutionState>, ExecutionStateCmp> m_statesSet;
	std::vector<std::shared_ptr<Expr>> m_eh_typeids;
	ExprOptimizer m_exprOptimizer;
	SolverManager m_solverManager;
};

#endif 
