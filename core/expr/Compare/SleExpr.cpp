#include "SleExpr.h"

SleExpr::SleExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind SleExpr::getKind()
{
	return ExprKind::SLE;
}
