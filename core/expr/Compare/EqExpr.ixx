export module Expr.EqExpr;

import Expr;

export class EqExpr : public Expr {
public:
	EqExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::EQ;
	}
};