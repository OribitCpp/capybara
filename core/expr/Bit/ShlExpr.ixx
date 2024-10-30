export module Expr.ShlExpr;

import Expr;

export class ShlExpr : public Expr {
public:
	ShlExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::SHL;
	}
};