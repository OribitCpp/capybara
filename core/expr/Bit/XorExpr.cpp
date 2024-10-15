#include "XorExpr.h"

XorExpr::XorExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right):Expr(0,0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind XorExpr::getKind()
{
	return ExprKind::XOR;
}
