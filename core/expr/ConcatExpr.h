#ifndef  CONCATEXPR_H
#define CONCATEXPR_H

#include "Expr.h"
#include <vector>

class ConcatExpr : public Expr {
public:
	ConcatExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right);


	virtual ExprKind getKind() override;
	static std::shared_ptr<Expr> concat(const std::vector<std::shared_ptr<Expr>> &exprs);
};


#endif // ! 
