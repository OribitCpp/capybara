#ifndef ADDRESSSPACE_H
#define ADDRESSSPACE_H

#include <cstdint>
#include <map>
#include <memory>
#include "runtime/ObjectState.h"
#include "memory/MemoryObject.h"

class ExecutionState;
class AddressSpace {
public:
	AddressSpace() {}
	AddressSpace(AddressSpace& other);
	AddressSpace& operator=(const AddressSpace&) = delete;
	void bindObject(const std::shared_ptr<MemoryObject> &mo, std::shared_ptr<ObjectState> &os);
	void unbindObject(const std::shared_ptr<MemoryObject> &mo);
	bool resolveOne(const std::shared_ptr<ExecutionState>&state, const std::shared_ptr<ConstantExpr>& addr, const std::shared_ptr<MemoryObject>& object);
private:
	uint32_t m_cowKey = 1;
	std::map<const std::shared_ptr<MemoryObject>, std::shared_ptr<ObjectState>,MemoryObjectPtrCmp> m_memoryMap;
};

#endif // !AddressSpace_h
