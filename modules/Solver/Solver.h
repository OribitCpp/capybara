#ifndef  SOLVER_BASE_H
#define SOLVER_BASE_H

#include <chrono>
#include <cstdint>
#include <vector>
#include "expr/Expr.h"
#include "expr/ConstantExpr.h"

enum class SolverType
{
	ST_INVALID,
	ST_Z3,
	ST_STP
};

enum SolverValidity
{
	True = 1,
	False = -1,
	Unknown = 0
};

class SolverBase {
public:
	SolverBase() {}
	virtual ~SolverBase() {}
	void setTimeOut(std::chrono::steady_clock::duration value) {
		m_timeOut = value;
	}

	std::chrono::steady_clock::duration getTimeOut() const {
		return m_timeOut;
	}

	bool evaluate(const std::vector<std::shared_ptr<Expr>> constaints, std::shared_ptr<Expr> expr, SolverValidity& result);
	bool mustBeTrue(const std::vector<std::shared_ptr<Expr>> constaints, std::shared_ptr<Expr> expr, bool& result);
	bool getValue(const std::vector<std::shared_ptr<Expr>> constaints, std::shared_ptr<Expr> expr, std::shared_ptr<ConstantExpr>& result);

	
	virtual SolverType getType() const { return SolverType::ST_INVALID; }

protected:
	virtual bool computeTruth(const std::vector<std::shared_ptr<Expr>>& contrains, std::shared_ptr<Expr> expr, bool result) = 0;
	virtual bool computeValue(const std::vector<std::shared_ptr<Expr>>& contrains, std::shared_ptr<Expr> expr, std::shared_ptr<ConstantExpr>& result) = 0;
	virtual bool computeValidity(const std::vector<std::shared_ptr<Expr>>& contrains, std::shared_ptr<Expr> expr, SolverValidity &result);
private:
	std::chrono::steady_clock::duration m_timeOut;
};

#endif // 