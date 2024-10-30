export module Application;


#import <memory>;
#import <llvm/IR/LLVMContext.h>;

import Core;


class Application {
public:
    Application(int argc, char* argv[]);
    void init();
    void ready();
    void execute();
    void finish(){}
    void exit();

protected:

private:
    void parseConsoleParameters(int argc, char* argv[]);
    void processInput();

public:
private:
    int argc;
    char** argv;

    std::shared_ptr<llvm::LLVMContext> m_llvmContext;
    std::unique_ptr<ModuleWrapper> m_moduleStorage;
};

