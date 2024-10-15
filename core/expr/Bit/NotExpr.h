#ifndef NOTEXPR_H
#define NOTEXPR_H

#include "expr/Expr.h"

class NotExpr : public Expr {
public:
	NotExpr(const std::shared_ptr<Expr> &base);

	virtual ExprKind getKind() override;
};

#endif // !NOTEXPR_h
