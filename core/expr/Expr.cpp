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

bool isPowerOfTwo(uint64_t x)
{
	if (x == 0) return 0;
	return !(x & (x - 1));
};

uint32_t withoutRightmostBit(uint32_t x) {
	return x & (x - 1);
}

uint32_t indexOfSingleBit(uint32_t x) {
	assert(withoutRightmostBit(x) == 0);
	uint32_t res = 0;
	if (x & 0xFFFF0000) res += 16;
	if (x & 0xFF00FF00) res += 8;
	if (x & 0xF0F0F0F0) res += 4;
	if (x & 0xCCCCCCCC) res += 2;
	if (x & 0xAAAAAAAA) res += 1;
	assert(res < 32);
	assert((UINT32_C(1) << res) == x);
	return res;
}

unsigned indexOfSingleBit(uint64_t x) {
	assert((x & (x - 1)) == 0);
	unsigned res = indexOfSingleBit((uint32_t)(x | (x >> 32)));
	if (x & (UINT64_C(0xFFFFFFFF) << 32))
		res += 32;
	assert(res < 64);
	assert((UINT64_C(1) << res) == x);
	return res;
}
