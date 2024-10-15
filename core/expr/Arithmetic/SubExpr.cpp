#include "SubExpr.h"

SubExpr::SubExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind SubExpr::getKind()
{
	return ExprKind::SUB;
}