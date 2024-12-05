#include "SolverManager.h"
#include <chrono>
#include "statistic/StatisticManager.h"
#include "Z3Solver.h"

SolverManager::SolverManager() {
	m_solversMap[SolverType::ST_Z3] = std::make_shared<Z3Solver>();
}

bool SolverManager::mayBeTrue(const std::vector<std::shared_ptr<Expr>>& constraints, std::shared_ptr<Expr> expr, bool& result, std::chrono::steady_clock::duration& duration)
{
	bool res;
	if (!mustBeFalse(constraints, expr, res, duration))
		return false;
	result = !res;
	return true;
}

bool SolverManager::mayBeFalse(const std::vector<std::shared_ptr<Expr>>& constraints, std::shared_ptr<Expr> expr, bool& result, std::chrono::steady_clock::duration& duration)
{
	bool res;
	if (!mustBeTrue(constraints, expr, res, duration))
		return false;
	result = !res;
	return true;
}

bool SolverManager::mustBeTrue(const std::vector<std::shared_ptr<Expr>>& constraints, std::shared_ptr<Expr> expr, bool& result, std::chrono::steady_clock::duration& duration)
{
	++StatisticManager::queries;
	// Fast path, to avoid timer and OS overhead.
	if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(expr)) {
		result = CE->isTrue() ? true : false;
		return true;
	}

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	//if (simplifyExprs)
	//	expr = ConstraintManager::simplifyExpr(constraints, expr);

	bool success = m_solversMap[SolverType::ST_Z3]->mustBeTrue(constraints, expr, result);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	std::chrono::steady_clock::duration delta = end - begin;
	duration += delta;

	return success;
}

bool SolverManager::mustBeFalse(const std::vector<std::shared_ptr<Expr>>& constraints, std::shared_ptr<Expr> expr, bool& result, std::chrono::steady_clock::duration& duration)
{
	return mustBeTrue(constraints, Expr::createIsZero(expr), result, duration);
}

bool SolverManager::evaluate(const std::vector<std::shared_ptr<Expr>>& constraints, std::shared_ptr<Expr> expr, SolverValidity& result, std::chrono::steady_clock::duration& duration)
{
	++StatisticManager::queries;
	// Fast path, to avoid timer and OS overhead.
	if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(expr)) {
		result = CE->isTrue() ? SolverValidity::True : SolverValidity::False;
		return true;
	}

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	/*if (simplifyExprs)
		expr = ConstraintManager::simplifyExpr(constraints, expr);*/

	bool success = m_solversMap[SolverType::ST_Z3]->evaluate(constraints, expr, result);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	std::chrono::steady_clock::duration delta = end - begin;
	duration += delta;

	return success;
}

bool SolverManager::getValue(const std::vector<std::shared_ptr<Expr>>& constraints, std::shared_ptr<Expr> expr, std::shared_ptr<ConstantExpr>& result, std::chrono::steady_clock::duration& duration)
{
	++StatisticManager::queries;
	// Fast path, to avoid timer and OS overhead.
	if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(expr)) {
		result = CE;
		return true;
	}

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	/*if (simplifyExprs)
		expr = ConstraintManager::simplifyExpr(constraints, expr);*/

	bool success = m_solversMap[SolverType::ST_Z3]->getValue(constraints, expr, result);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	std::chrono::steady_clock::duration delta = end - begin;
	duration += delta;

	return success;
}

void SolverManager::setTimeOut(std::chrono::steady_clock::duration duration)
{
	m_solversMap[SolverType::ST_Z3]->setTimeOut(duration);
}
