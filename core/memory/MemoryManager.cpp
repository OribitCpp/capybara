#include "MemoryManager.h"
#include "MemoryObject.h"

MemoryManager::MemoryManager():m_usedSize(0){

}

MemoryManager::~MemoryManager()
{
}

MemoryManager& MemoryManager::instance()
{
	static MemoryManager manager;
	return manager;
}


std::shared_ptr<MemoryObject> MemoryManager::alloc(uint64_t size)
{
	if (size == 0) return nullptr;
	uint64_t address = reinterpret_cast<uint64_t>(new char[size]);
	std::shared_ptr<MemoryObject> result = std::make_shared<MemoryObject>(address, size);
	if (result) {
		instance().m_usedSize += size;
	}
	return result;
}

void MemoryManager::markFree(MemoryObject* address)
{
	if (m_memoryMap.find(address) != m_memoryMap.end()) {
		m_usedSize -= m_memoryMap[address]->size;
		m_memoryMap.erase(address);
	}
}
