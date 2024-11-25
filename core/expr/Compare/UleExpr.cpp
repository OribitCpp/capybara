#include "UleExpr.h"

UleExpr::UleExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind UleExpr::getKind()
{
	return ExprKind::ULE;
}
