export module Expr.SltExpr;

import Expr;

export class SltExpr : public Expr {
public:
	SltExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::SLT;
	}
};