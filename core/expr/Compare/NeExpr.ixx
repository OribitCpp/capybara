export module Expr.NeExpr;

import Expr;

export class NeExpr : public Expr {
public:
	NeExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::NE;
	}
};