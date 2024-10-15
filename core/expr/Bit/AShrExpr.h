#ifndef ASHREXPR_H
#define ASHREXPR_H

#include "expr/Expr.h"

class AShrExpr : public Expr {
public:
	AShrExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif // !ASHREXPR_H
