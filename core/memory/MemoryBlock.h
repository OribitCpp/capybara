#ifndef MEMORY_BLOCK_H
#define MEMORY_BLOCK_H

class MemoryBlock {
	friend class MemoryMananger;
public:
	MemoryBlock(unsigned int size);
	~MemoryBlock();

	MemoryBlock(const MemoryBlock& other);
	MemoryBlock&  operator=(const MemoryBlock& other);
	MemoryBlock(const MemoryBlock&&) = delete;
	MemoryBlock& operator=(const MemoryBlock&&) = delete;
	
private:
	void* m_ptr = nullptr;
	unsigned int m_size = 0;
};

#endif // !MEMORY_BLOCK_H
