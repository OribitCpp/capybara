#ifndef LSHREXPR_H
#define LSHREXPR_H

#include "expr/Expr.h"
class LShrExpr : public Expr {
public:
	LShrExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif // !LSHREXPR_H
