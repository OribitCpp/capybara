#ifndef  UDIVEXPR_H
#define UDIVEXPR_H

#include "expr/Expr.h"
class UDivExpr : public Expr {
public:
	UDivExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind();
};

#endif // ! ADDEXPR_H
