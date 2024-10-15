#ifndef  SREMEXPR_H
#define SREMEXPR_H

#include "expr/Expr.h"
class SRemExpr : public Expr {
public:
	SRemExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind();
};

#endif // ! ADDEXPR_H
