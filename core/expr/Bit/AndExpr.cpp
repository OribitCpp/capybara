#include "AndExpr.h"

AndExpr::AndExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right):Expr(0,0)
{
	leftExpr = left;
	rightExpr = right;
}

ExprKind AndExpr::getKind()
{
	return ExprKind::AND;
}
