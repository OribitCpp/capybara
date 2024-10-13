#ifndef  CONSTANTEXPR_H
#define CONSTANTEXPR_H

#include "Expr.h"

class ConstantExpr:public Expr {
public:
	ConstantExpr(unsigned int bitNumber, uint64_t value);

	virtual ExprKind getKind() override;
	virtual std::unique_ptr<Expr> clone() override;

	std::unique_ptr<ConstantExpr> Sge(const std::unique_ptr<ConstantExpr>& RHS);
private:
};

#endif // ! ConstantExpr_h
