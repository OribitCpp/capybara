export module XorExpr;

import Expr;

export class XorExpr : public Expr {
public:
	XorExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::XOR;
	}
};