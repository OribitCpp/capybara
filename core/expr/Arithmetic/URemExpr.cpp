#include "URemExpr.h"

URemExpr::URemExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind URemExpr::getKind()
{
	return ExprKind::UREM;
}