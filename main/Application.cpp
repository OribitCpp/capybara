#include "Application.h"
#include <iostream>

#include "ModuleWrapper.h"
#include "FileLoader.h"
#include "runtime/ExecutionState.h"

#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Module.h>

Application::Application(int argc, char* argv[])
{
	parseConsoleParameters(argc, argv);
}

void Application::init() {
	Logger::info("Appplication Init Stage");
	llvm::InitializeNativeTarget();
	m_llvmContext = std::make_shared<llvm::LLVMContext>();
}
void Application::ready() {
	std::vector<std::unique_ptr<llvm::Module>> modules;
	FileLoader fileLoader(*m_llvmContext);
	fileLoader.load("D:/capybara/get_sign.bc", modules);

	std::unique_ptr<llvm::Module> finalModule = ModuleWrapper::linkModules(modules);
	
	m_moduleStorage = std::make_unique<ModuleWrapper>(finalModule);
	m_moduleStorage->optimiseWithPass();
}

void Application::execute()
{
	std::shared_ptr<FunctionWrapper> mainFunc = m_moduleStorage->getFunction("main");
	ExecutionState executionState(mainFunc);
}


void Application::parseConsoleParameters(int argc, char* argv[])
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

void Application::processInput()
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

void Application::exit() {
	llvm::llvm_shutdown();
}
