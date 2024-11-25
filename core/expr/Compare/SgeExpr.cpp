#include "SgeExpr.h"

SgeExpr::SgeExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right):Expr(0,0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind SgeExpr::getKind()
{
	return ExprKind::SGE;
}
