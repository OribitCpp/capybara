#ifndef SLEEXPR_H
#define SLEEXPR_H

#include "expr/Expr.h"

class SleExpr :public Expr {
public:
	SleExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif