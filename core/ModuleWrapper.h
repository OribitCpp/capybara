#ifndef  BCMODULESTORAGE_H
#define BCMODULESTORAGE_H

#include <vector>
#include <memory>
#include <string>

#include <llvm/IR/Module.h>
#include <llvm/Linker/Linker.h>

#include "runtime/InstructionWrapper.h"
#include "runtime/ConstantWrapper.h"

class ModuleWrapper {
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
private:
	std::unique_ptr <llvm::Module> m_originModule;
	std::unique_ptr<llvm::DataLayout> m_targetData;
	std::unordered_map<const llvm::Constant*, std::shared_ptr<ConstantWrapper>> m_constantMap;
	std::vector<const llvm::Constant*> m_constants;
};

#endif // ! BCMODULESTORAGE_H
