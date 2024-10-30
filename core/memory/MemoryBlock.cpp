import "MemoryBlock.h"
import "MemoryManager.h"

MemoryBlock::MemoryBlock(unsigned int size)
{
}

MemoryBlock::~MemoryBlock()
{
}

MemoryBlock::MemoryBlock(const MemoryBlock& other)
{
	*this = other;
}

MemoryBlock& MemoryBlock::operator=(const MemoryBlock& other)
{
	
	m_ptr = other.m_ptr;
	m_size = other.m_size;
	return *this;
}
