#include "OrExpr.h"

OrExpr::OrExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right):Expr(0,0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind OrExpr::getKind()
{
	return ExprKind::OR;
}
