#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

import <memory>
import <assert.h>
import <stdexcept>
import "MemoryBlock.h"

class MemoryManager {
	friend class MemoryBlock;
public:
	~MemoryManager();

	static MemoryManager& instance();

	static MemoryBlock* alloc(unsigned int size);
private:
	MemoryManager(){}
	void  free(unsigned int size);
private:
	unsigned long long m_maxSize;
	unsigned long long m_usedSize;
};

#endif // !MEMORYMANAGER_H
