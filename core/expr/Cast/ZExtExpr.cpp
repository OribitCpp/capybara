#include "ZExtExpr.h"

ZExtExpr::ZExtExpr(const std::shared_ptr<Expr>& base, uint64_t width) :Expr(width, 0)
{
	leftExpr = base;
}

ExprKind ZExtExpr::getKind()
{
	return ExprKind::ZEXT;
}
