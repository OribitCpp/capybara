export module Expr.ConcatExpr;

import Expr;

class NotOptimizedExpr:public Expr {
public:
	NotOptimizedExpr(const std::shared_ptr<Expr> base):Expr(0,0)
	{
		rightExpr = base;
	}

	virtual ExprKind getKind() override
	{
		return ExprKind::NOTOPTIMIZED;
	}
};

