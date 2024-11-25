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
	std::shared_ptr<MemoryObject> result = std::make_shared<MemoryObject>(size);
	if (result) {
		instance().m_usedSize += size;
	}
	return result;
}

void MemoryManager::markFree(MemoryObject* object)
{
	if (m_memoryMap.find(object) != m_memoryMap.end()) {
		m_usedSize -= object->size;
		m_memoryMap.erase(object);
	}
}
