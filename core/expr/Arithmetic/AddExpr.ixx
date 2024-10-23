
export module AddExpr;

import Expr;

export class AddExpr : public Expr {
public:
	AddExpr(const std::shared_ptr<Expr>& left, const std::shared_ptr<Expr>& right) :Expr(0, 0)
	{
		leftExpr = left;
		rightExpr = right;
	 }

	virtual ExprKind getKind() {
		return ExprKind::ADD;
	}
};