#ifndef MEMORY_OBJECT_H
#define MEMORY_OBJECT_H

#include <stdint.h>
#include <llvm/IR/Value.h>
#include "expr/ConstantExpr.h"

class MemoryManager;
class MemoryObject {
public:
    MemoryObject(uint64_t size);


    MemoryObject(uint64_t _size, unsigned _alignment,
        bool _isLocal, bool _isGlobal, bool _isFixed,
        const llvm::Value* _allocSite)
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

    ~MemoryObject();
    MemoryObject(const MemoryObject& b) = delete;
    MemoryObject& operator=(const MemoryObject& b) = delete;

    std::shared_ptr<ConstantExpr> getBaseExpr() const;
public:
	uint32_t id = 0;
    uint64_t address = 0;

	/// size in bytes
	uint64_t size = 0;
	unsigned alignment;
	mutable std::string name = "unname";

	bool isLocal = false;
	mutable bool isGlobal = false;
    bool isUserSpecified = false;
	bool isFixed = false;
    const llvm::Value* allocSite = nullptr;
private:
    static uint32_t counter;
};

#endif // !MEMORY_OBJECT_H
