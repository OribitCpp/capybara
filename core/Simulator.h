#ifndef  SIMULATOR_H
#define SIMULATOR_H

#include "runtime/FunctionWrapper.h"

class Simulator {
public:
	Simulator();
	~Simulator();

	Simulator(const Simulator&) = delete;
	Simulator& operator=(const Simulator&) = delete;

	void runMainFunction(FunctionWrapper *function);
};

#endif // ! simulator_h
