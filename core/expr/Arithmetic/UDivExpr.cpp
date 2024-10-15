#include "UDivExpr.h"

UDivExpr::UDivExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind UDivExpr::getKind()
{
	return ExprKind::UDIV;
}