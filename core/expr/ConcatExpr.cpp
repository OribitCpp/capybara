#include "ConcatExpr.h"
#include "Logger.h"

ConcatExpr::ConcatExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right):Expr(0,0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind ConcatExpr::getKind()
{
	return ExprKind::CONCAT;
}

std::shared_ptr<Expr> ConcatExpr::concat(const std::vector<std::shared_ptr<Expr>>& exprs)
{
	if (exprs.size() <= 0) {
		Logger::error("concat empty expr array!");
		return nullptr;
	};
	if (exprs.size() == 1) return exprs.back();
	std::shared_ptr<Expr> result = std::make_shared<ConcatExpr>(exprs[0], exprs[1]);
	for (uint32_t index = 2; index < exprs.size(); index++) {
		result = std::make_shared<ConcatExpr>(exprs[index], result);
	}
	return result;
}
