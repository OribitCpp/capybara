#ifndef SGTEXPR_H
#define SGTEXPR_H

#include "expr/Expr.h"

class SgtExpr :public Expr {
public:
	SgtExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif