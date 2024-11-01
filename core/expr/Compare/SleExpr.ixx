export module Expr.SleExpr;

import Expr;

export class SleExpr : public Expr {
public:
	SleExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::SLE;
	}
};