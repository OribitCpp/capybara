#ifndef  REFCOUNTER_H
#define REFCOUNTER_H

#include <atomic>
class  RefCounter {
public:
	RefCounter();
	~RefCounter();
	void increase();
	unsigned int decrease();
	unsigned int value();
private:
	std::atomic_uint32_t m_count;
};


#endif // ! REFCOUNTER_H
