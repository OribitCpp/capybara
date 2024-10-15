#ifndef NOTOPTIMIZEDEXPR_H
#define NOTOPTIMIZEDEXPR_H

#include "Expr.h"

class NotOptimizedExpr:public Expr {
public:
	NotOptimizedExpr(const std::shared_ptr<Expr> base);

	virtual ExprKind getKind() override;
};

#endif // !NOTOPTIMIZEDEXPR_H
