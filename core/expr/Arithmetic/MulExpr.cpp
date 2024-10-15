#include "MulExpr.h"

MulExpr::MulExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right):Expr(0,0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind MulExpr::getKind()
{
	return ExprKind::MUL;
}
