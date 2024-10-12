#include "MemoryManager.h"

MemoryManager::~MemoryManager()
{
}

MemoryManager& MemoryManager::instance()
{
	static MemoryManager manager;
	return manager;
}

MemoryBlock* MemoryManager::alloc(unsigned int size)
{
	return nullptr;
}

void MemoryManager::free(unsigned int size)
{
	if (size > m_usedSize) {
		throw std::runtime_error("free excess mem");
	}
	m_usedSize -= size;
}
