#ifndef  CONSTANTEXPR_H
#define CONSTANTEXPR_H

#include "Expr.h"

class ConstantExpr:public Expr {
public:
	ConstantExpr(unsigned int bitNumber, uint64_t value);
	ConstantExpr(const llvm::APInt& value);

	virtual ExprKind getKind() override;

	std::shared_ptr<ConstantExpr> Concat(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Extract(unsigned offset, unsigned int width);
	std::shared_ptr<ConstantExpr> ZExt(unsigned int width);
	std::shared_ptr<ConstantExpr> SExt(unsigned int width);
	std::shared_ptr<ConstantExpr> Add(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Sub(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Mul(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> UDiv(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> SDiv(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> URem(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> SRem(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> And(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Or(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Xor(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Shl(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> LShr(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> AShr(const std::shared_ptr<ConstantExpr>& other);

	std::shared_ptr<ConstantExpr> Eq(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Ne(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Ult(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Ule(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Ugt(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Uge(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Slt(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Sle(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Sgt(const std::shared_ptr<ConstantExpr>& other);
	std::shared_ptr<ConstantExpr> Sge(const std::shared_ptr<ConstantExpr>& other);

	std::shared_ptr<ConstantExpr> Neg();
	std::shared_ptr<ConstantExpr> Not();
private:
};


#endif // ! ConstantExpr_h
