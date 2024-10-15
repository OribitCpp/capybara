#include "Expr.h"

unsigned int Expr::s_total = 0;


Expr::Expr(unsigned int bitNumber, uint64_t value) :m_value(bitNumber,value)
{
	s_total++;
}

Expr::Expr(const llvm::APInt& value):m_value(value)
{
}

Expr::~Expr()
{
	s_total--;
}

ExprKind Expr::getKind()
{
	return ExprKind::INVALID;
}
