export module Expr.SgtExpr;

import Expr;

export class SgtExpr : public Expr {
public:
	SgtExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::SGT;
	}
};