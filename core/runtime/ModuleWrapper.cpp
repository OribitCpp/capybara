#include "ModuleWrapper.h"
#include "runtime/FunctionWrapper.h"
#include "Logger.h"

ModuleWrapper::ModuleWrapper(std::unique_ptr<llvm::Module>& module):m_originModule(std::move(module))
{
    m_targetData = std::make_unique<llvm::DataLayout>(m_originModule.get());
}

bool ModuleWrapper::linkTwoModule(llvm::Module& dest, std::unique_ptr<llvm::Module> src)
{
    std::string identifier = src->getModuleIdentifier();
    bool result = llvm::Linker::linkModules(dest, std::move(src));
    if (!result)  Logger::error("failed to load module:  ", identifier);
    return result;
}

std::unique_ptr<llvm::Module> ModuleWrapper::linkModules(std::vector<std::unique_ptr<llvm::Module>>& modules)
{
    std::unique_ptr<llvm::Module> composite =  std::move(modules.back());
    modules.pop_back();

     for(std::unique_ptr<llvm::Module> &tmp : modules)
    {
         bool result = linkTwoModule(*composite.get(), std::move(tmp));
         if (!result) return nullptr;
    }

    return composite;
}

void ModuleWrapper::optimiseWithPass()
{
}

unsigned int ModuleWrapper::getConstantID(llvm::Constant* constant, std::shared_ptr<InstructionWrapper>& instructionWrapper)
{
    if (std::shared_ptr<ConstantWrapper> result = getConstant(constant)) {
        return result->ID;
    }
    unsigned int id = m_constants.size();
    auto instance = std::make_shared<ConstantWrapper>(constant, id, instructionWrapper);
    m_constantMap[constant] = instance;
    m_constants.push_back(constant);
    return id;
}

std::shared_ptr<ConstantWrapper> ModuleWrapper::getConstant(const llvm::Constant* constant)
{
    auto iter = m_constantMap.find(constant);
    if (iter != m_constantMap.end()) {
        return iter->second;
    }
    return std::shared_ptr<ConstantWrapper>();
}

std::shared_ptr<FunctionWrapper> ModuleWrapper::getFunction(const std::string& name)
{
    if (m_functionMap.find(name) != m_functionMap.end()) {
        return m_functionMap[name];
    }
    llvm::Function* function = m_originModule->getFunction(name);
    if (function != nullptr) {
        std::shared_ptr<FunctionWrapper> funcWapper = std::make_shared<FunctionWrapper>(function, shared_from_this());
        m_functionMap[name] = funcWapper;
        return funcWapper;
    }
    return nullptr;
}

std::shared_ptr<FunctionWrapper> ModuleWrapper::getFunction(const llvm::Function* func)
{
    return getFunction(func->getName().str());
}

llvm::Module* ModuleWrapper::getOriginPtr()
{
    return m_originModule.get();
}

std::uint64_t ModuleWrapper::getTypeSize(llvm::Type *type)
{
    return m_targetData->getTypeStoreSize(type);
}

std::uint64_t ModuleWrapper::getTypeSizeInBits(llvm::Type* type)
{
    return m_targetData->getTypeSizeInBits(type);
}

llvm::TypeSize ModuleWrapper::getTypeAllocSize(llvm::Type* type)
{
    return m_targetData->getTypeAllocSize(type);
}

const llvm::StructLayout* ModuleWrapper::getStructLayout(llvm::StructType* type)
{
    return m_targetData->getStructLayout(type);
}
