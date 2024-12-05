#include "AddressSpace.h"
#include "Logger.h"
#include <cassert>

AddressSpace::AddressSpace(AddressSpace& other)
{
	m_cowKey =++other.m_cowKey;
}

void AddressSpace::bindObject(const std::shared_ptr<MemoryObject>& mo, std::shared_ptr<ObjectState>& os)
{
	if (os->m_copyOnWriteOwner != 0) {
		Logger::error("object already has owner");
		assert(0);
	}
	m_memoryMap.emplace(mo, os);
}

void AddressSpace::unbindObject(const std::shared_ptr<MemoryObject>& mo)
{
	m_memoryMap.erase(mo);
}
