#include "MemoryObject.h"
#include "MemoryManager.h"

uint32_t MemoryObject::counter = 0;

MemoryObject::MemoryObject(uint64_t size)
    : id(counter++),
    address(0),
    size(size),
    alignment(0),
    isFixed(true),
    allocSite(nullptr) {
    address = reinterpret_cast<uint64_t>(new char[size]);
}

MemoryObject::~MemoryObject()
{
	MemoryManager::instance().markFree(this);
    char* pointer = reinterpret_cast<char*>(address);
    delete[] pointer;
}

std::shared_ptr<ConstantExpr> MemoryObject::getBaseExpr() const
{
    return std::make_shared<ConstantExpr>(address, sizeof(void*));
}
