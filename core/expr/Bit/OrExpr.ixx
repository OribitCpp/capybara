export module Expr.OrExpr;

import Expr;

export class OrExpr : public Expr {
public:
	OrExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::OR;
	}
};