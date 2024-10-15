#include "LShrExpr.h"

LShrExpr::LShrExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right):Expr(0,0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind LShrExpr::getKind()
{
	return ExprKind::LSHR;
}
