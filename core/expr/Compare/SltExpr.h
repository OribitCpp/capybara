#ifndef SLTEXPR_H
#define SLTEXPR_H

#include "expr/Expr.h"

class SltExpr :public Expr {
public:
	SltExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif