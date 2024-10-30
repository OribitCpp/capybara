export module Expr.URemExpr;

import Expr;

export class URemExpr : public Expr {
public:
	URemExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::UREM;
	}
};