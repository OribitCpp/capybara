#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include <filesystem>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/MemoryBuffer.h>

class FileLoader {
public:
	FileLoader(llvm::LLVMContext& context);
	~FileLoader();
	FileLoader(const FileLoader&) = delete;
	FileLoader& operator=(const FileLoader&) = delete;
	FileLoader(const FileLoader&&) = delete;
	FileLoader& operator=(const FileLoader&&) = delete;

	 void load(const std::string& fileName, std::vector<std::unique_ptr<llvm::Module>> &result);
private:
	void loadArchive(llvm::MemoryBufferRef &buffer,std::vector<std::unique_ptr<llvm::Module>> &result);
private:
	std::string m_fileName;
	llvm::LLVMContext& m_context;
};

#endif // !FILE_LOADER_H
