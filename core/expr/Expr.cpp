#include "Expr.h"
#include "expr/Compare/EqExpr.h"
#include "expr/ConstantExpr.h"

uint64_t Expr::s_total = 0;


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

uint64_t Expr::getWidth() const
{
	return m_value.getBitWidth();
}

std::shared_ptr<Expr> Expr::createIsZero(std::shared_ptr<Expr> expr)
{
	return std::make_shared<EqExpr>(expr, std::make_shared<ConstantExpr>(expr->getWidth(),0));
}

const llvm::fltSemantics* Expr::fpWidthToSemantics(uint32_t width)
{
	switch (width)
	{
	case 32:
		return &llvm::APFloat::IEEEsingle();
	case 64:
		return &llvm::APFloat::IEEEdouble();
	case 80:
		return &llvm::APFloat::x87DoubleExtended();
	}
	return nullptr;
}

bool Expr::operator==(const Expr& other) const
{
	if (this->getWidth() != other.getWidth()) return false;
	if (this->m_value != other.m_value) return false;
	if (this->leftExpr != other.leftExpr) return false;
	if (this->rightExpr != other.rightExpr) return false;
	return true;
}

llvm::raw_ostream& operator<<(llvm::raw_ostream& os, const Expr& expr)
{
	os << "Expr";
	return os;
}
