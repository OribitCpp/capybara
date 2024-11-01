export module Expr.UgtExpr;

import Expr;

export class UgtExpr : public Expr {
public:
	UgtExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::UGT;
	}
};