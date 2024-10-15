#include "SelectExpr.h"

SelectExpr::SelectExpr(const std::shared_ptr<Expr>& cond, const std::shared_ptr<Expr>& trueExpr, const std::shared_ptr<Expr>& falseExpr):Expr(0,0)
{
	condition = cond;
	leftExpr = trueExpr;
	rightExpr = falseExpr;
}

ExprKind SelectExpr::getKind()
{
	return ExprKind::SELECT;
}
