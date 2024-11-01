export module ModuleWrapper;

import <vector>;
import <memory>;
import <string>;
import <iostream>;

import <llvm/IR/Module.h>;
import <llvm/Linker/Linker.h>;

export class InstructionWrapper;
export class ConstantWrapper;
export class FunctionWrapper;

export class ModuleWrapper:public std::enable_shared_from_this<ModuleWrapper> {
public:
	ModuleWrapper(std::unique_ptr<llvm::Module>& llvmModule);
	~ModuleWrapper() {}

	ModuleWrapper(const ModuleWrapper&) = delete;
	ModuleWrapper& operator=(const ModuleWrapper&) = delete;
	ModuleWrapper(const ModuleWrapper&&) = delete;
	ModuleWrapper& operator=(const ModuleWrapper&&) = delete;

	static bool linkTwoModule(llvm::Module& dest, std::unique_ptr<llvm::Module> src);
	static std::unique_ptr<llvm::Module>  linkModules(std::vector<std::unique_ptr<llvm::Module>>& modules);

	void optimiseWithPass();
	unsigned int getConstantID(llvm::Constant* constant,const std::shared_ptr<InstructionWrapper>& instructionWrapper);
	std::shared_ptr<ConstantWrapper> getConstant(const llvm::Constant* constant);
	std::shared_ptr<FunctionWrapper> getFunction(std::string name);
private:
	std::unique_ptr <llvm::Module> m_originModule;
	std::unique_ptr<llvm::DataLayout> m_targetData;

	std::vector<const llvm::Constant*> m_constants;

	std::unordered_map<const llvm::Constant*, std::shared_ptr<ConstantWrapper>> m_constantMap;
	std::unordered_map<llvm::Function*, std::shared_ptr<FunctionWrapper>> m_functionMap;

};