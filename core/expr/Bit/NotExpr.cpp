#include "NotExpr.h"

NotExpr::NotExpr(const std::shared_ptr<Expr>& base):Expr(0,0)
{
	rightExpr = base;
}

ExprKind NotExpr::getKind()
{
	return ExprKind::NOT;
}
