#ifndef ADDEXPR_H
#define ADDEXPR_H

#include "expr/Expr.h"

class AndExpr: public Expr {
public:
	AndExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};

#endif // !ADDEXPR_H
