#ifndef  EXTERNALDISPATCHER_H
#define EXTERNALDISPATCHER_H
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/LLVMContext.h>
#include <string>
#include <vector>
#include <memory>

class ExternalDispatcher {
public:
	ExternalDispatcher(llvm::LLVMContext& context);
	~ExternalDispatcher();
	void* resolvesSymbol(const std::string& name);
	std::string getFreshModuleID();
private:
	llvm::ExecutionEngine* m_excutionEngine = nullptr;
	llvm::LLVMContext& m_context;
	std::vector<std::string> m_moduleIDs;
	int m_lastError = 0;
};

#endif // ! ExternalDispatcher_h
