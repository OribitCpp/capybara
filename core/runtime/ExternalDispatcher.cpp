#include "ExternalDispatcher.h"
#include <sstream>
#include "Logger.h"

#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/TargetSelect.h>


ExternalDispatcher::ExternalDispatcher(llvm::LLVMContext& context):m_context(context)
{
	std::unique_ptr<llvm::Module>	singleDispatchModule = std::make_unique<llvm::Module>(getFreshModuleID(), m_context);
	std::string error;
	m_excutionEngine = llvm::EngineBuilder(std::move(singleDispatchModule)).setErrorStr(&error).setEngineKind(llvm::EngineKind::JIT).create();
	if (m_excutionEngine == nullptr) {
		Logger::error("unable to make jit: {}", error);
		abort();
	}
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmParser();
	llvm::InitializeNativeTargetAsmPrinter();

	llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
}

ExternalDispatcher::~ExternalDispatcher()
{
	delete m_excutionEngine;
}

void* ExternalDispatcher::resolvesSymbol(const std::string& name)
{
	const char* str = name.c_str();
	if (str[0] == 1) // asm specifier, skipped
		++str;

	void* addr = llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(str);
	if (addr)
		return addr;

	// If it has an asm specifier and starts with an underscore we retry
	// without the underscore. I (DWD) don't know why.
	if (name[0] == 1 && str[0] == '_') {
		++str;
		addr = llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(str);
	}

	return addr;
}

std::string ExternalDispatcher::getFreshModuleID()
{
	static uint64_t counter = 0;
	std::stringstream ss;
	ss << "ExternalDispatcherModule_" << counter++;
	m_moduleIDs.push_back(ss.str());
	return m_moduleIDs.back();
}
