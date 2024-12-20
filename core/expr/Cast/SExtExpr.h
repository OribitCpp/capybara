#ifndef SEXTEXPR_H
#define SEXTEXPR_H

#include "expr/Expr.h"

class SExtExpr: public Expr {
public:
	SExtExpr(const std::shared_ptr<Expr>& base, uint64_t width);
	virtual ExprKind getKind() override;

};


#endif // !1
