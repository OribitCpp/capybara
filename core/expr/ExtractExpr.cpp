#include "ExtractExpr.h"


ExtractExpr::ExtractExpr(const std::shared_ptr<Expr>& baseExpr, unsigned int start, unsigned offset): 
	Expr(start, offset)
{
	rightExpr = baseExpr;
}

ExtractExpr::~ExtractExpr()
{
}

ExprKind ExtractExpr::getKind()
{
	return ExprKind::EXTRACT;
}
