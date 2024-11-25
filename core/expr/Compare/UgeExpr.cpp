#include "UgeExpr.h"

UgeExpr::UgeExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind UgeExpr::getKind()
{
	return ExprKind::UGE;
}
