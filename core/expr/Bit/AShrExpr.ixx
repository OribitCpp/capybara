
export module Expr.AShrExpr;

import Expr;

export class AShrExpr : public Expr {
public:
	AShrExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::ASHR;
	}
};