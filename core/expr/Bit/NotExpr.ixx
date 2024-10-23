export module URemExpr;

import Expr;

export class NotExpr : public Expr {
public:
	NotExpr(const std::shared_ptr<Expr>& base) :Expr(0, 0)
	{
		rightExpr = base;
	}

	virtual ExprKind getKind() {
		return ExprKind::NOT;
	}
};