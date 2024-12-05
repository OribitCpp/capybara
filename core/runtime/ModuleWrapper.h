#ifndef  BCMODULESTORAGE_H
#define BCMODULESTORAGE_H

#include <vector>
#include <memory>
#include <string>
#include <set>

#include <llvm/IR/Module.h>
#include <llvm/Linker/Linker.h>

#include "runtime/InstructionWrapper.h"
#include "runtime/ConstantWrapper.h"

struct FunctionWrapper;
class ModuleWrapper : public std::enable_shared_from_this<ModuleWrapper>{
public:
	ModuleWrapper(std::unique_ptr<llvm::Module>& module);
	~ModuleWrapper() {}

	ModuleWrapper(const ModuleWrapper&) = delete;
	ModuleWrapper& operator=(const ModuleWrapper&) = delete;
	ModuleWrapper(const ModuleWrapper&&) = delete;
	ModuleWrapper& operator=(const ModuleWrapper&&) = delete;

	static bool linkTwoModule(llvm::Module& dest, std::unique_ptr<llvm::Module> src);
	static std::unique_ptr<llvm::Module>  linkModules(std::vector<std::unique_ptr<llvm::Module>>& modules);

	void optimiseWithPass();
	unsigned int getConstantID(llvm::Constant* constant, std::shared_ptr<InstructionWrapper>& instructionWrapper);
	std::shared_ptr<ConstantWrapper> getConstant(const llvm::Constant* constant);
	std::shared_ptr<FunctionWrapper> getFunction(const std::string& name);
	std::shared_ptr<FunctionWrapper> getFunction(const llvm::Function *func);
	llvm::Module* getOriginPtr();
	std::uint64_t getTypeSize(llvm::Type *type);
	std::uint64_t getTypeSizeInBits(llvm::Type *type);
	uint64_t getPrefTypeAlign(llvm::Type *type);
	llvm::TypeSize getTypeAllocSize(llvm::Type* type);
	const llvm::StructLayout* getStructLayout(llvm::StructType* type);
	const std::set<llvm::Function*>& getInternalFunctions() const { return m_internalFunctions;}
private:
	std::unique_ptr <llvm::Module> m_originModule;
	std::unique_ptr<llvm::DataLayout> m_targetData;
	std::unordered_map<const llvm::Constant*, std::shared_ptr<ConstantWrapper>> m_constantMap;
	std::unordered_map<std::string, std::shared_ptr<FunctionWrapper>> m_functionMap;
	std::vector<const llvm::Constant*> m_constants;
	std::set<llvm::Function*> m_internalFunctions;
};

#endif // ! BCMODULESTORAGE_H
