#include "RefCounter.h"

RefCounter::RefCounter():m_count(1)
{
}

RefCounter::~RefCounter()
{
}

void RefCounter::increase()
{
	m_count.fetch_add(1, std::memory_order_relaxed);
}

unsigned int RefCounter::decrease()
{
	m_count.fetch_sub(1, std::memory_order_relaxed);
	return m_count.load();
}

unsigned int RefCounter::value()
{
	return m_count.load();
}
