#include "ConcatExpr.h"

ConcatExpr::ConcatExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right):Expr(0,0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind ConcatExpr::getKind()
{
	return ExprKind::CONCAT;
}
