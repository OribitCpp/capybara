export module Core:ConstantExpr;

import :Expr;

export class ConstantExpr:public Expr {
public:
	ConstantExpr(unsigned int bitNumber, uint64_t value):Expr(bitNumber,value)
	{

	}
	ConstantExpr(const llvm::APInt& value):Expr(value)
	{

	}

	virtual ExprKind getKind() override
	{
		return ExprKind::CONSTANT;
	}

	std::shared_ptr<ConstantExpr> Concat(const std::shared_ptr<ConstantExpr>& other)
{
	unsigned int totalWidth = m_value.getBitWidth() + other->m_value.getBitWidth();
	llvm::APInt tmpValue(m_value);
	tmpValue = tmpValue.zext(totalWidth);
	tmpValue <<= other->m_value.getBitWidth();
	tmpValue |= llvm::APInt(other->m_value).zext(totalWidth);

	return std::make_shared<ConstantExpr>(tmpValue);
}

std::shared_ptr<ConstantExpr> Extract(unsigned offset, unsigned int width)
{
	return std::make_shared<ConstantExpr>(m_value.ashr(offset).zextOrTrunc(width));
}

std::shared_ptr<ConstantExpr> ZExt(unsigned int width)
{
	return std::make_shared<ConstantExpr>(m_value.zextOrTrunc(width));
}

std::shared_ptr<ConstantExpr> SExt(unsigned int width)
{
	return std::make_shared<ConstantExpr>(m_value.sextOrTrunc(width));
}

std::shared_ptr<ConstantExpr> Add(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value + other->m_value);
}

std::shared_ptr<ConstantExpr> Sub(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value - other->m_value);
}

std::shared_ptr<ConstantExpr> Mul(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value * other->m_value);
}

std::shared_ptr<ConstantExpr> UDiv(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value.udiv(other->m_value));
}

std::shared_ptr<ConstantExpr> SDiv(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value.sdiv(other->m_value));
}

std::shared_ptr<ConstantExpr> URem(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value.urem(other->m_value));
}

std::shared_ptr<ConstantExpr> SRem(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value.srem(other->m_value));
}

std::shared_ptr<ConstantExpr> And(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value & other->m_value);
}

std::shared_ptr<ConstantExpr> Or(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value | other->m_value);
}

std::shared_ptr<ConstantExpr> Xor(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value ^ other->m_value);
}

std::shared_ptr<ConstantExpr> Shl(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value.shl(other->m_value));
}

std::shared_ptr<ConstantExpr> LShr(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value.lshr(other->m_value));
}

std::shared_ptr<ConstantExpr> AShr(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(m_value.ashr(other->m_value));
}

std::shared_ptr<ConstantExpr> Eq(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(llvm::APInt(1, m_value == other->m_value));
}

std::shared_ptr<ConstantExpr> Ne(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(llvm::APInt(1,m_value != other->m_value));
}

std::shared_ptr<ConstantExpr> Ult(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(llvm::APInt(1, m_value.ult(other->m_value)));
}

std::shared_ptr<ConstantExpr> Ule(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(llvm::APInt(1, m_value.ule(other->m_value)));
}

std::shared_ptr<ConstantExpr> Ugt(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(llvm::APInt(1, m_value.ugt(other->m_value)));
}

std::shared_ptr<ConstantExpr> Uge(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(llvm::APInt(1, m_value.uge(other->m_value)));
}

std::shared_ptr<ConstantExpr> Slt(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(llvm::APInt(1, m_value.slt(other->m_value)));
}

std::shared_ptr<ConstantExpr> Sle(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(llvm::APInt(1, m_value.sle(other->m_value)));
}

std::shared_ptr<ConstantExpr> Sgt(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(llvm::APInt(1, m_value.sgt(other->m_value)));
}

std::shared_ptr<ConstantExpr> Sge(const std::shared_ptr<ConstantExpr>& other)
{
	return std::make_shared<ConstantExpr>(llvm::APInt(1, m_value.sge(other->m_value)));
}

std::shared_ptr<ConstantExpr> Neg()
{
	return std::make_shared<ConstantExpr>(-m_value);
}

std::shared_ptr<ConstantExpr> Not()
{
	return std::make_shared<ConstantExpr>(~m_value);
}
private:
};
