#ifndef  MEMORYOBJECT_H
#define MEMORYOBJECT_H

import <llvm/IR/Value.h>
import <string>

class MemoryManager;

class MemoryObject {
	friend class MemoryManager;
public:
	MemoryObject(uint64_t address);
	MemoryObject(uint64_t address, unsigned int size, unsigned int alignment,
	bool m_isGlobal, bool isLocal,	bool sFixed, bool isUserSpecified, const llvm::Value* allocSite, MemoryManager* parent);
	
	MemoryObject(const MemoryObject* other) = delete;
	MemoryObject& operator=(const MemoryObject* other) = delete;

private:
	unsigned int ID;
	uint64_t m_address;
	unsigned int m_size;
	unsigned int m_alignment;
	mutable std::string m_name;
	mutable bool m_isGlobal = false;
	bool m_isLocal = false;
	bool m_isFixed = false;
	bool m_isUserSpecified = false;
	
	MemoryManager* m_parent;
	const llvm::Value* m_allocSite;
	static unsigned int  s_total;
};
#endif // ! 
