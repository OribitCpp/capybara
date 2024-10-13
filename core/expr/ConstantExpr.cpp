#include "ConstantExpr.h"

ConstantExpr::ConstantExpr(unsigned int bitNumber, uint64_t value):Expr(bitNumber,value){
}

ExprKind ConstantExpr::getKind()
{
	return ExprKind::CONSTANT;
}

std::unique_ptr<Expr> ConstantExpr::clone()
{
	return std::unique_ptr<Expr>();
}

std::unique_ptr<ConstantExpr> ConstantExpr::Sge(const std::unique_ptr<ConstantExpr>& RHS)
{
	return std::unique_ptr<ConstantExpr>();
}
