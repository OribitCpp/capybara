#include "AddExpr.h"

AddExpr::AddExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind AddExpr::getKind()
{
	return ExprKind::ADD;
}
