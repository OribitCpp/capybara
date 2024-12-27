#include "AddressSpace.h"
#include "Logger.h"
#include <cassert>
#include "ExecutionState.h"

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

bool AddressSpace::resolveOne(const std::shared_ptr<ExecutionState>& state, const std::shared_ptr<ConstantExpr>& addr, const std::shared_ptr<MemoryObject>& object)
{
    uint64_t address = addr->getZExtValue();
    MemoryObject hack(address);

    if (const auto res = objects.lookup_previous(&hack)) {
        const auto& mo = res->first;
        // Check if the provided address is between start and end of the object
        // [mo->address, mo->address + mo->size) or the object is a 0-sized object.
        if ((mo->size == 0 && address == mo->address) ||
            (address - mo->address < mo->size)) {
            result.first = res->first;
            result.second = res->second.get();
            return true;
        }
    }

    return false;
}
