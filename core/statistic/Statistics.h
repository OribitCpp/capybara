#ifndef STATISTICS_H
#define STATISTICS_H

#include <string>

class Statistics {
public:
	Statistics(std::string name);
	~Statistics() {}

	const std::string& name() const;
	uint64_t value() const;

	Statistics& operator++();
	Statistics operator++(int);
	Statistics& operator+=(uint64_t value);
private:
	std::string m_name;
	uint64_t m_value = 0;
};

#endif