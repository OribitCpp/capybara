#include "SltExpr.h"

SltExpr::SltExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind SltExpr::getKind()
{
	return ExprKind::SLT;
}
