#ifndef ZEXTEXPR_H
#define ZEXTEXPR_H

#include "expr/Expr.h"

class ZExtExpr : public Expr {
public:
	ZExtExpr(const std::shared_ptr<Expr>& base, unsigned int width);
	virtual ExprKind getKind() override;

};

#endif