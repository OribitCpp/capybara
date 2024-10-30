export module Expr.SDivExpr;

import Expr;

export class SDivExpr : public Expr {
public:
	SDivExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::SDIV;
	}
};