#ifndef  OREXPR_H
#define OREXPR_H

#include "expr/Expr.h"
class OrExpr : public Expr {
public:
	OrExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);
	virtual ExprKind getKind() override;
};


#endif // ! OREXPR_H
