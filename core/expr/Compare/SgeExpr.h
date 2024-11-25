#ifndef SGEEXPR_H
#define SGEEXPR_H

#include "expr/Expr.h"

class SgeExpr :public Expr {
public:
	SgeExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif