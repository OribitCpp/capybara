#ifndef  APPLICATION_H
#define APPLICATION_H

#include "Logger.h"
#include "ModuleWrapper.h"
#include <memory>
#include <llvm/IR/LLVMContext.h>

class Application {
public:
	Application(int argc, char* argv[]);
	void init();
	void ready();
	void execute();
	void finish(){}
	void exit();

	int status() { return m_status; }
	void setStatus(int value) { m_status = value; }
protected:
	virtual bool update();

private:
	void parseConsoleParameters(int argc, char* argv[]);
	void processInput();
private:
	int argc;
	char** argv;
	bool keepRun = true;
	int m_status = 0;

	std::shared_ptr<llvm::LLVMContext> m_llvmContext;
	std::unique_ptr<ModuleWrapper> m_moduleStorage;
};

#endif 
