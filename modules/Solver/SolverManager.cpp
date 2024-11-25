#include "SolverManager.h"

bool SolverManager::mayBeTrue(const std::vector<std::shared_ptr<Expr>>& constrants, std::shared_ptr<Expr>, bool& result, std::chrono::steady_clock::duration& duration)
{
	return false;
}

bool SolverManager::getValue(const std::vector<std::shared_ptr<Expr>>& constrants, std::shared_ptr<Expr>, std::shared_ptr<ConstantExpr>& result, std::chrono::steady_clock::duration& duration)
{
	return false;
}
