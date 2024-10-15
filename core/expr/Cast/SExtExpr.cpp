#include "SExtExpr.h"

SExtExpr::SExtExpr(const std::shared_ptr<Expr>& base, unsigned int width):Expr(width,0)
{
	rightExpr = base;
}

ExprKind SExtExpr::getKind()
{
	return ExprKind::SEXT;
}
