#include "ExtractExpr.h"


ExtractExpr::ExtractExpr(const std::shared_ptr<Expr>& baseExpr, uint64_t start, uint64_t offset):
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
