export module MemoryObject;
import <llvm/IR/Value.h>;
import <string>;

export class MemoryObject {
	friend class MemoryManager;
public:
	MemoryObject(uint64_t address):ID(++s_total),m_address(address),m_size(0),m_alignment(0),m_isFixed(true),
	m_parent(nullptr),m_allocSite(0)
	{
	}

	MemoryObject(uint64_t address, unsigned int size, unsigned int alignment, bool isGlobal, bool isLocal, bool isFixed, bool isUserSpecified, const llvm::Value* allocSite, MemoryManager* parent):ID(++s_total), m_address(address), m_size(size), m_alignment(alignment), m_isFixed(isFixed),
		m_isLocal(isLocal),m_isGlobal(isGlobal), m_parent(parent), m_allocSite(allocSite)
	{
	}
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
unsigned int  MemoryObject::s_total=0;
