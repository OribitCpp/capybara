#ifndef ULTEXPR_H
#define ULTEXPR_H

#include "expr/Expr.h"

class UltExpr :public Expr {
public:
	UltExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif