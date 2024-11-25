#ifndef Z3_RESOLVER_H
#define Z3_RESOLVER_H

#include "Solver.h"
#include <z3++.h>

class Z3Solver : public Solver {
public:
	Z3Solver();
	~Z3Solver();
private:
	z3::context m_z3Context;
};

#endif