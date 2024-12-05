#ifndef  SOLVER_MANAGER_H
#define SOLVER_MANAGER_H

#include <memory>
#include <vector>
#include <chrono>
#include <unordered_map>
#include "expr/Expr.h"
#include "expr/ConstantExpr.h"
#include "Z3Solver.h"

class SolverManager {
public:
	SolverManager();
	bool mayBeTrue(const std::vector<std::shared_ptr<Expr>>& constraints, std::shared_ptr<Expr>, bool& result,	std::chrono::steady_clock::duration& duration);
	bool mayBeFalse(const std::vector<std::shared_ptr<Expr>>& constraints, std::shared_ptr<Expr>, bool& result,	std::chrono::steady_clock::duration& duration);
	bool mustBeTrue(const std::vector<std::shared_ptr<Expr>>& constraints, std::shared_ptr<Expr>, bool& result,	std::chrono::steady_clock::duration& duration);
	bool mustBeFalse(const std::vector<std::shared_ptr<Expr>>& constraints, std::shared_ptr<Expr>, bool& result,	std::chrono::steady_clock::duration& duration);
	bool evaluate(const std::vector<std::shared_ptr<Expr>>& constraints, std::shared_ptr<Expr>, SolverValidity& result,	std::chrono::steady_clock::duration& duration);
	bool getValue(const std::vector<std::shared_ptr<Expr>>& constraints, std::shared_ptr<Expr>, std::shared_ptr<ConstantExpr>& result,	std::chrono::steady_clock::duration& duration);

	void setTimeOut(std::chrono::steady_clock::duration dutation);
private:
	std::unordered_map<SolverType, std::shared_ptr<Solver>> m_solversMap;
};

#endif // ! SolverManager_h
