#ifndef  BCMODULESTORAGE_H
#define BCMODULESTORAGE_H

#include <vector>
#include <memory>
#include <string>

#include <llvm/IR/Module.h>
#include <llvm/Linker/Linker.h>

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

private:
	std::unique_ptr <llvm::Module> m_originModule;
	std::unique_ptr<llvm::DataLayout> m_targetData;

};

#endif // ! BCMODULESTORAGE_H
