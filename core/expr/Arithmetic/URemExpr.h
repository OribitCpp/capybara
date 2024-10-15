#ifndef  UREMEXPR_H
#define UREMEXPR_H

#include "expr/Expr.h"
class URemExpr : public Expr {
public:
	URemExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind();
};

#endif // ! ADDEXPR_H
