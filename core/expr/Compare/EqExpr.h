#ifndef EQEXPR_H
#define EQEXPR_H

#include "expr/Expr.h"

class EqExpr:public Expr {
public:
	EqExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;

	bool isEqual() const;
};

#endif