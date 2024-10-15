#include "SDivExpr.h"

SDivExpr::SDivExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind SDivExpr::getKind()
{
	return ExprKind::SDIV;
}