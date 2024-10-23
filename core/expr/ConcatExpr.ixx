export module ConcatExpr;

import Expr;

export class ConcatExpr : public Expr {
public:
	ConcatExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::CONCAT;
	}
};