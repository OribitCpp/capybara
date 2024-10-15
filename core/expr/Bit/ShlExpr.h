#ifndef SHLEXPR_H
#define SHLEXPR_H


#include "expr/Expr.h"
class ShlExpr : public Expr {
public:
	ShlExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif // !SHLEXPR_H

