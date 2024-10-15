#include "SRemExpr.h"

SRemExpr::SRemExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind SRemExpr::getKind()
{
	return ExprKind::SREM;
}