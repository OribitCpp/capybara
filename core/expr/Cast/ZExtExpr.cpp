#include "ZExtExpr.h"

ZExtExpr::ZExtExpr(const std::shared_ptr<Expr>& base, unsigned int width) :Expr(width, 0)
{
}

ExprKind ZExtExpr::getKind()
{
	return ExprKind::ZEXT;
}
