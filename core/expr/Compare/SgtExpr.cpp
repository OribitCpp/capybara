#include "SgtExpr.h"

SgtExpr::SgtExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind SgtExpr::getKind()
{
	return ExprKind::SGT;
}
