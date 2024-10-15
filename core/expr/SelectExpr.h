#ifndef  SELECTEXPR_H
#define SELECTEXPR_H

#include "Expr.h"
class SelectExpr: public Expr{
public:
	SelectExpr(const std::shared_ptr<Expr> &cond, const std::shared_ptr<Expr> &trueExpr,const std::shared_ptr<Expr> &falseExpr);
	
	virtual ExprKind getKind() override;
	std::shared_ptr<Expr> condition;
};

#endif // ! SELECTEXPR_H
