#include "UgtExpr.h"

UgtExpr::UgtExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind UgtExpr::getKind()
{
	return ExprKind::UGT;
}
