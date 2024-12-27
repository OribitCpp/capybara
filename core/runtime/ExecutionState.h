#ifndef  EXECUTIONSTATE_H
#define EXECUTIONSTATE_H
#include <memory>
#include <vector>
#include <set>
#include <chrono>
#include "FunctionWrapper.h"
#include "StackFrame.h"
#include "memory/MemoryObject.h"
#include "runtime/ObjectState.h"
#include "runtime/ArrayObject.h"
#include "runtime/AddressSpace.h"

enum class BranchType {
	NONE,                                                       
	Conditional,                                                    
	Indirect,                                                     
	Switch,                                                            
	Call,                                                            
	MemOp,                                                            
	ResolvePointer,                                                   
	Alloc,                                                             
	Realloc,                                                        
	Free,                                                            
	GetVal,                                                          
	END, 
};



class ExecutionState  {
public:
	ExecutionState(std::shared_ptr<FunctionWrapper> &func);
	ExecutionState(ExecutionState&);
	ExecutionState& operator=(const ExecutionState&) = delete;
	ExecutionState(ExecutionState&&) = delete;
	ExecutionState& operator=(ExecutionState&&) = delete;

	void pushFrame(std::vector<std::shared_ptr<InstructionWrapper>>::iterator pc, std::shared_ptr<FunctionWrapper> func);
	std::shared_ptr<StackFrame> getStackFrame();
	void popFrame();

	void saveInAllocator(const std::shared_ptr<MemoryObject> &object);
	void remove(const std::shared_ptr<MemoryObject> &object);
	uint64_t ID() { return m_id; }
	size_t getStackSize() const;
	void bindLocal(const std::shared_ptr<InstructionWrapper> wrapper, const std::shared_ptr<Expr>& expr);
	std::shared_ptr<Expr> eval(const std::shared_ptr<InstructionWrapper>& wrapper, uint32_t index);

	const std::vector<std::shared_ptr<StackFrame>>& getStack() const { return m_stack; }
	void dumpStack(llvm::raw_ostream& out) const;
	std::shared_ptr<Expr> getArgumentExpr(const std::shared_ptr<FunctionWrapper>& func, uint32_t index);
	void bindArgument(const std::shared_ptr<FunctionWrapper>& func, uint32_t index, const std::shared_ptr<Expr>& expr);
	std::shared_ptr<ObjectState>bindObjectInState(const std::shared_ptr<MemoryObject>& memObject, const std::shared_ptr<ArrayObject>& array = nullptr);
	void unbindObject(const std::shared_ptr<MemoryObject>& memObj);
	void addConstraint(std::shared_ptr<Expr> expr);

	std::shared_ptr<ExecutionState> branch();
	bool resolveOne(const std::shared_ptr<ConstantExpr>& addr, const std::shared_ptr<MemoryObject>& object);
public:
	std::vector<std::shared_ptr<InstructionWrapper>>::iterator PC;
	std::vector<std::shared_ptr<InstructionWrapper>>::iterator prevPC;
	uint32_t incomingBBIndex = 0;
	std::vector<std::shared_ptr<Expr>> constraints;
	std::chrono::steady_clock::duration duraion;
	uint32_t depth = 0;
	bool coveredNew = false;
	std::map<std::string, std::set<uint32_t>> coveredLines;
	std::map<std::shared_ptr<Expr>, std::shared_ptr<ConstantExpr>> base_addrs;
private:
	void setID();
private:
	std::set<std::shared_ptr<MemoryObject>> m_heapAllocator;
	std::set<std::shared_ptr<MemoryObject>> m_stackAllocator;
	std::vector<std::shared_ptr<StackFrame>> m_stack;
	static uint64_t s_total;
	uint64_t m_id;
	AddressSpace m_addressSpace;
};

struct ExecutionStateCmp {
	bool operator()(const std::shared_ptr<ExecutionState>& left, const std::shared_ptr<ExecutionState>& right) const{
		return left->ID() < right->ID();
	}
};

#endif // ! EXECUTIONSTATE_H
