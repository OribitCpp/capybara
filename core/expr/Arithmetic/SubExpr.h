#ifndef  SUBEXPR_H
#define SUBEXPR_H

#include "expr/Expr.h"
class SubExpr : public Expr {
public:
	SubExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind();
};

#endif // ! ADDEXPR_H
