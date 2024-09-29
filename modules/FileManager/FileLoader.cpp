#include "FileLoader.h"
#include "Logger.h"
#include <llvm/BinaryFormat/Magic.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Object/ObjectFile.h>
#include <llvm/Object/Archive.h>

using namespace llvm;

FileLoader::FileLoader(const std::string& fileName, llvm::LLVMContext& context):m_fileName(fileName),m_context(context)
{
}

FileLoader::~FileLoader()
{
}

void FileLoader::load(std::vector<std::unique_ptr<llvm::Module>>& result)
{
	ErrorOr<std::unique_ptr<MemoryBuffer>> bufferErr = MemoryBuffer::getFileOrSTDIN(m_fileName);
	if (bufferErr.getError()) {
		Logger::error("load file {} failed", m_fileName);
		return;
	}

	MemoryBufferRef buffer = bufferErr.get()->getMemBufferRef();
	file_magic magic = identify_magic(buffer.getBuffer());
	switch (magic)
	{
	case file_magic::bitcode: {
		SMDiagnostic err;
		result.emplace_back(parseIR(buffer, err, m_context));
		return;
	}

	case file_magic::archive: {
		loadArchive(buffer, result);
		return;
	}
	default:
		break;
	}
}

void FileLoader::loadArchive(MemoryBufferRef &buffer, std::vector<std::unique_ptr<llvm::Module>>& result)
{
	Expected<std::unique_ptr<object::Binary>> archOwner = object::createBinary(buffer, &m_context);
	if (!archOwner) {
		std::error_code ec = errorToErrorCode(archOwner.takeError());
		Logger::error("load file {} failed, {}", m_fileName, ec.message().c_str());
		return;
	}
	object::Binary* arch = archOwner.get().get();
	object::Archive* archive = dyn_cast<object::Archive>(arch);
	if (archive) {
		auto Err = Error::success();
		for (auto AI = archive->child_begin(Err); AI != archive->child_end(); ++AI)
		{

		}
	}
}
