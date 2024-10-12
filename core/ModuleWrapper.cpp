#include "ModuleWrapper.h"
#include <iostream>

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
