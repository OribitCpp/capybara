export module Expr.UgeExpr;

import Expr;

export class UgeExpr : public Expr {
public:
	UgeExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::UGE;
	}
};