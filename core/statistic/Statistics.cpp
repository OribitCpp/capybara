#include "Statistics.h"

Statistics::Statistics( std::string name):m_name(name) {

}

const std::string& Statistics::name() const
{
	return m_name;
}

uint64_t Statistics::value() const
{
	return m_value;
}

Statistics& Statistics::operator++()
{
	++m_value;
	return *this;
}

Statistics Statistics::operator++(int)
{
	Statistics tmp = *this;
	m_value++;
	return tmp;
}

Statistics& Statistics::operator+=(uint64_t value)
{
	m_value += value;
	return *this;
}
