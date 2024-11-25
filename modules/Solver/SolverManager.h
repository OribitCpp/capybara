#ifndef  SOLVERMANAGER_H
#define SOLVERMANAGER_H

#include <memory>
#include <vector>
#include <chrono>
#include "expr/Expr.h"
#include "expr/ConstantExpr.h"
class SolverManager {
public:
	bool mayBeTrue(const std::vector<std::shared_ptr<Expr>>& constrants, std::shared_ptr<Expr>, bool& result,	std::chrono::steady_clock::duration& duration);
	bool getValue(const std::vector<std::shared_ptr<Expr>>& constrants, std::shared_ptr<Expr>, std::shared_ptr<ConstantExpr>& result,	std::chrono::steady_clock::duration& duration);
};
#endif // ! SolverManager_h
