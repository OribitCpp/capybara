export module Expr.UltExpr;

import Expr;

export class UltExpr : public Expr {
public:
	UltExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::ULT;
	}
};