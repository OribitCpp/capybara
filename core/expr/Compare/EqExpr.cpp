#include "EqExpr.h"

EqExpr::EqExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right):Expr(0,0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind EqExpr::getKind()
{
	return ExprKind::EQ;
}

bool EqExpr::isEqual() const
{
	return leftExpr == rightExpr;
}
