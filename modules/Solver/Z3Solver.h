#ifndef Z3_SOLVER_H
#define Z3_SOLVER_H

#include "Solver.h"
#include "Z3Builder.h"

class Z3Solver : public SolverBase {
public:
	Z3Solver();
	~Z3Solver();

	virtual SolverType getType() const{ return SolverType::ST_Z3; }
protected:
	virtual bool computeTruth(const std::vector<std::shared_ptr<Expr>>& contrains, std::shared_ptr<Expr> expr, bool result)  override;
	virtual bool computeValue(const std::vector<std::shared_ptr<Expr>>& contrains, std::shared_ptr<Expr> expr, std::shared_ptr<ConstantExpr>& result)  override;

private:
	Z3Builder m_builder;
};

#endif