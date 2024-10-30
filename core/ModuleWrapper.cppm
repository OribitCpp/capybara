module ModuleWrapper;
import <iostream>

ModuleWrapper::ModuleWrapper(std::unique_ptr<llvm::Module>& module):m_originModule(std::move(module))
{
    m_targetData = std::make_unique<llvm::DataLayout>(m_originModule.get());
}

bool ModuleWrapper::linkTwoModule(llvm::Module& dest, std::unique_ptr<llvm::Module> src)
{
    std::string identifier = src->getModuleIdentifier();
    bool result = llvm::Linker::linkModules(dest, std::move(src));
    if (!result)  std::cerr << "failed to load module:  "<< identifier;
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

std::shared_ptr<FunctionWrapper> ModuleWrapper::getFunction(std::string name)
{
    llvm::Function* func = m_originModule->getFunction(name);
    if (m_functionMap.find(func) == m_functionMap.end()) {
        m_functionMap[func] = std::make_shared<FunctionWrapper>(func, shared_from_this());
    }
    return m_functionMap[func] ;
}
