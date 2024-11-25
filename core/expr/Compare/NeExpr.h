#ifndef NEEXPR_H
#define NEEXPR_H

#include "expr/Expr.h"

class NeExpr :public Expr {
public:
	NeExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif