#include "SExtExpr.h"

SExtExpr::SExtExpr(const std::shared_ptr<Expr>& base, uint64_t width):Expr(width,0)
{
	leftExpr = base;
}

ExprKind SExtExpr::getKind()
{
	return ExprKind::SEXT;
}
