#include "NeExpr.h"

NeExpr::NeExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind NeExpr::getKind()
{
	return ExprKind::NE;
}
