export module Expr.LShrExpr;

import Expr;

export class LShrExpr : public Expr {
public:
	LShrExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::LSHR;
	}
};