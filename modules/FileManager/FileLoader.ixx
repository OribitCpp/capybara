export module FileManager.FileLoader;

import <filesystem>;
import <string>;
import <memory>;
import <vector>;

import <llvm/IR/Module.h>;
import <llvm/IR/LLVMContext.h>;
import <llvm/Support/MemoryBuffer.h>;
import <llvm/BinaryFormat/Magic.h>;
import <llvm/Support/SourceMgr.h>;
import <llvm/IRReader/IRReader.h>;
import <llvm/Object/ObjectFile.h>;
import <llvm/Object/Archive.h>;
import <iostream>;
import Logger;

export class FileLoader {
public:
	FileLoader(llvm::LLVMContext& context):m_context(context){}
	~FileLoader(){};
	FileLoader(const FileLoader&) = delete;
	FileLoader& operator=(const FileLoader&) = delete;
	FileLoader(const FileLoader&&) = delete;
	FileLoader& operator=(const FileLoader&&) = delete;

	void load(const std::string& fileName, std::vector<std::unique_ptr<llvm::Module>> &result){

		llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> bufferErr = llvm::MemoryBuffer::getFileOrSTDIN(fileName);
		std::error_code error = bufferErr.getError();
		if (error) {
			std::cerr << "load file" << fileName <<"failed: "<< error.message() << std::endl;
			return;
		}

		llvm::MemoryBufferRef buffer = bufferErr.get()->getMemBufferRef();
		llvm::file_magic magic = identify_magic(buffer.getBuffer());
		switch (magic)
		{
		case llvm::file_magic::bitcode: {
				llvm::SMDiagnostic err;
				result.emplace_back(parseIR(buffer, err, m_context));
				return;
		}

		case llvm::file_magic::archive: {
				loadArchive(buffer, result);
				return;
		}
		default:
			break;
		}
	}
private:
	void loadArchive(llvm::MemoryBufferRef &buffer,std::vector<std::unique_ptr<llvm::Module>> &result)
	{
		llvm::Expected<std::unique_ptr<llvm::object::Binary>> archOwner = llvm::object::createBinary(buffer, &m_context);
		if (!archOwner) {
			std::error_code ec = errorToErrorCode(archOwner.takeError());
			Logger::error("load file failed, {}", ec.message().c_str());
			return;
		}
		llvm::object::Binary* arch = archOwner.get().get();
		llvm::object::Archive* archive = llvm::dyn_cast<llvm::object::Archive>(arch);
		if (archive) {
			auto Err = llvm::Error::success();
			for (auto AI = archive->child_begin(Err); AI != archive->child_end(); ++AI)
			{

			}
		}
	}
private:
	llvm::LLVMContext& m_context;
};
