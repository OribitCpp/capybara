export module MemoryManager;

import <memory>;
import <assert.h>;
import <stdexcept>;

export class MemoryManager {
	friend class MemoryBlock;
public:
	~MemoryManager(){};

	static MemoryManager& instance()
	{
		static MemoryManager manager;
		return manager;
	}

	static MemoryBlock* alloc(unsigned int size)
	{
		return nullptr;
	}
private:
	MemoryManager(){}
	void  free(unsigned int size)
	{
		if (size > m_usedSize) {
			throw std::runtime_error("free excess mem");
		}
		m_usedSize -= size;
	}
private:
	unsigned long long m_maxSize;
	unsigned long long m_usedSize;
};