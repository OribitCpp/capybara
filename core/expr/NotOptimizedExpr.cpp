#include "NotOptimizedExpr.h"

NotOptimizedExpr::NotOptimizedExpr(const std::shared_ptr<Expr> base):Expr(0,0)
{
	rightExpr = base;
}

ExprKind NotOptimizedExpr::getKind()
{
	return ExprKind::NOTOPTIMIZED;
}


