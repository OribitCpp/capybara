#ifndef  ADDEXPR_H
#define ADDEXPR_H

#include "expr/Expr.h"
class AddExpr : public Expr {
public:
	AddExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind();
};

#endif // ! ADDEXPR_H
