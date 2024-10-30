export module Expr.SRemExpr;

import Expr;

export class SRemExpr : public Expr {
public:
	SRemExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::SREM;
	}
};