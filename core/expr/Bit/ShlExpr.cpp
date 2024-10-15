#include "ShlExpr.h"

ShlExpr::ShlExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right):Expr(0,0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind ShlExpr::getKind()
{
	return ExprKind::SHL;
}
