#ifndef UGTEXPR_H
#define UGTEXPR_H

#include "expr/Expr.h"

class UgtExpr :public Expr {
public:
	UgtExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif