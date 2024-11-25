#include "UltExpr.h"

UltExpr::UltExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind UltExpr::getKind()
{
	return ExprKind::ULE;
}
