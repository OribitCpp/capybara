#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <stdint.h>
#include <memory>
#include <assert.h>
#include <stdexcept>
#include <unordered_map>
#include <llvm/IR/Value.h>

class MemoryObject;
class MemoryManager {
	friend class MemoryBlock;
public:
	~MemoryManager();

	static MemoryManager& instance();

	static std::shared_ptr<MemoryObject> alloc(uint64_t size);
	void markFree(MemoryObject* address);
private:
	MemoryManager();
private:
	uint64_t m_usedSize = 0;
	std::unordered_map<MemoryObject*, std::shared_ptr<MemoryObject>> m_memoryMap;
};

#endif // !MEMORYMANAGER_H
