#include "NotOptimizedExpr.h"

NotOptimizedExpr::NotOptimizedExpr(const std::shared_ptr<Expr> base):Expr(0,0)
{
	leftExpr = base;
}

ExprKind NotOptimizedExpr::getKind()
{
	return ExprKind::NOTOPTIMIZED;
}


