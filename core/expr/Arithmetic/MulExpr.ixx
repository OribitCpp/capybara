export module Expr.MulExpr;

import Expr;

export class MulExpr : public Expr {
public:
	MulExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::ADD;
	}
};
