export module Expr.UleExpr;

import Expr;

export class UleExpr : public Expr {
public:
	UleExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::ULE;
	}
};