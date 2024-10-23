export module AndExpr;

import Expr;

export class AndExpr : public Expr {
public:
	AndExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	}

	virtual ExprKind getKind() {
		return ExprKind::AND;
	}
};