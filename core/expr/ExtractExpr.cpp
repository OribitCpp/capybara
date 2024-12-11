#include "ExtractExpr.h"


ExtractExpr::ExtractExpr(const std::shared_ptr<Expr>& baseExpr, uint64_t start, uint64_t offset):
	Expr(0, 0),
	m_offset(offset),
	m_start(start)
{
	leftExpr = baseExpr;
}

ExtractExpr::~ExtractExpr()
{
}

ExprKind ExtractExpr::getKind()
{
	return ExprKind::EXTRACT;
}

