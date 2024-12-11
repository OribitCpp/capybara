#include "NotExpr.h"

NotExpr::NotExpr(const std::shared_ptr<Expr>& base):Expr(0,0)
{
	leftExpr = base;
}

ExprKind NotExpr::getKind()
{
	return ExprKind::NOT;
}
