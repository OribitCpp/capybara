#ifndef  SOLVER_H
#define SOLVER_H

#include <cstdint>

class Solver {
public:

	void setTimeOut(uint32_t value) {
		m_timeOut = value;
	}

	uint32_t getTimeOut() const {
		return m_timeOut;
	}
private:
	uint32_t m_timeOut = 0;
};

#endif // ! SOLVER_H
