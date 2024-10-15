#ifndef XOREXPR_H
#define XOREXPR_H



#include "expr/Expr.h"
class XorExpr : public Expr {
public:
	XorExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};


#endif // !LSHREXPR_H
