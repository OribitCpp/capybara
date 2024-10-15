#ifndef  MULEXPR_H
#define MULEXPR_H

#include "expr/Expr.h"
class MulExpr : public Expr {
public:
	MulExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind();
};

#endif // ! ADDEXPR_H
