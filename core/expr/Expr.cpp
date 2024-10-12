#include "Expr.h"

unsigned int Expr::s_total = 0;

Expr::Expr()
{
	s_total++;
}

Expr::~Expr()
{
	s_total--;
}
