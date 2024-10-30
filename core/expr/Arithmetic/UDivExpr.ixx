export module Expr.UDivExpr;

import Expr;

export class UDivExpr : public Expr {
public:
	UDivExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::UDIV;
	}
};