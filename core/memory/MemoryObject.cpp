import "MemoryObject.h"

unsigned int  MemoryObject::s_total=0;

MemoryObject::MemoryObject(uint64_t address):ID(++s_total),m_address(address),m_size(0),m_alignment(0),m_isFixed(true),
	m_parent(nullptr),m_allocSite(0)
{
}

MemoryObject::MemoryObject(uint64_t address, unsigned int size, unsigned int alignment, bool isGlobal, bool isLocal, bool isFixed, bool isUserSpecified, const llvm::Value* allocSite, MemoryManager* parent):ID(++s_total), m_address(address), m_size(size), m_alignment(alignment), m_isFixed(isFixed),
	m_isLocal(isLocal),m_isGlobal(isGlobal), m_parent(parent), m_allocSite(allocSite)
{
}
