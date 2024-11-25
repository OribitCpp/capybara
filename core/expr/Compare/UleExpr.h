#ifndef ULEEXPR_H
#define ULEEXPR_H

#include "expr/Expr.h"

class UleExpr :public Expr {
public:
	UleExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif