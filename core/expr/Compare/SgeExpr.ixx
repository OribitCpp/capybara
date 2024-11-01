export module Expr.SgeExpr;

import Expr;

export class SgeExpr : public Expr {
public:
	SgeExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::SGE;
	}
};