export module Application;


import <memory>;
import <llvm/IR/LLVMContext.h>;
import <iostream>;
import <llvm/Support/ManagedStatic.h>;
import <llvm/Support/TargetSelect.h>;
import <llvm/IR/Module.h>;

import ModuleWrapper;
import FunctionWrapper;
import FileManager.FileLoader;
import ExecutionState;
import Logger;

export class Application {
public:
    Application(int argc, char* argv[])
    {
        parseConsoleParameters(argc, argv);
    }
    void init() {
        Logger::info("Appplication Init Stage");
        llvm::InitializeNativeTarget();
        m_llvmContext = std::make_shared<llvm::LLVMContext>();
    }
    void ready() {
        std::vector<std::unique_ptr<llvm::Module>> modules;
        FileLoader fileLoader(*m_llvmContext);
        fileLoader.load("D:/capybara/get_sign.bc", modules);

        std::unique_ptr<llvm::Module> finalModule = ModuleWrapper::linkModules(modules);

        m_moduleStorage = std::make_unique<ModuleWrapper>(finalModule);
        m_moduleStorage->optimiseWithPass();
    }

    void execute()
    {
        std::shared_ptr<FunctionWrapper> mainFunc = m_moduleStorage->getFunction("main");
        ExecutionState executionState(mainFunc);
    }
    void finish(){

    }

    void exit() {
        llvm::llvm_shutdown();
    }

protected:

private:
    void parseConsoleParameters(int argc, char* argv[])
    {
        // -proj-dir: the project directory
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i],"-help")==0 || strcmp(argv[i],"-h") == 0) {
            }
            else if (argv[i] == "") {

            }
        }
    }
    void processInput()
    {
        std::string inputData;
        std::getline(std::cin, inputData);

        if (inputData == "exit")
        {
        }
        else if (inputData == "help") {
            std::cout << "All of parameters are displayed as follow:" << std::endl;
        }
    }

public:
private:
    int argc;
    char** argv;

    std::shared_ptr<llvm::LLVMContext> m_llvmContext;
    std::unique_ptr<ModuleWrapper> m_moduleStorage;
};

