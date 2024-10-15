#include "AShrExpr.h"

AShrExpr::AShrExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right):Expr(0,0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind AShrExpr::getKind()
{
	return ExprKind::ASHR;
}
