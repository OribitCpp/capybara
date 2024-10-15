#ifndef  SDIVEXPR_H
#define SDIVEXPR_H

#include "expr/Expr.h"
class SDivExpr : public Expr {
public:
	SDivExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind();
};

#endif // ! ADDEXPR_H
