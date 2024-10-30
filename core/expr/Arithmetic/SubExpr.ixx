export module Expr.SubExpr;

import Expr;

export class SubExpr : public Expr {
public:
	SubExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::SUB;
	}
};