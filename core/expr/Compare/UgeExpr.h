#ifndef UGEEXPR_H
#define UGEEXPR_H

#include "expr/Expr.h"

class UgeExpr :public Expr {
public:
	UgeExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif