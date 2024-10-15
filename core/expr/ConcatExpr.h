#ifndef  CONCATEXPR_H
#define CONCATEXPR_H

#include "Expr.h"

class ConcatExpr : public Expr {
public:
	ConcatExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);


	virtual ExprKind getKind() override;
};


#endif // ! 
