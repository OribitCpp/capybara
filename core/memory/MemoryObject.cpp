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

MemoryObject::MemoryObject(uint64_t _size, unsigned _alignment, bool _isLocal, bool _isGlobal, bool _isFixed, const llvm::Value* _allocSite)
    : id(counter++),
    address(0),
    size(_size),
    alignment(_alignment),
    name("unnamed"),
    isLocal(_isLocal),
    isGlobal(_isGlobal),
    isFixed(_isFixed),
    isUserSpecified(false),
    allocSite(_allocSite) {
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

bool MemoryObjectCmp::operator()(const MemoryObject& a, const MemoryObject& b) const
{
    return a.address < b.address;
}

bool MemoryObjectPtrCmp::operator()(const std::shared_ptr<MemoryObject>& a, const std::shared_ptr<MemoryObject>& b) const
{
    MemoryObjectCmp func;
    return func(*a,*b);
}
