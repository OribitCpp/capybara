export module SExtExpr;

import Expr;

export class SExtExpr : public Expr {
public:
	SExtExpr(const std::shared_ptr<Expr>& base, unsigned int width) :Expr(width,0)
	{
		rightExpr = base;
	}

	virtual ExprKind getKind() {
		return ExprKind::SEXT;
	}
};