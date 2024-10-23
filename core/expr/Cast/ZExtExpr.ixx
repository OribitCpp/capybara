export module Core:SExtExpr;

import :Expr;

export class ZExtExpr : public Expr {
public:
	ZExtExpr(const std::shared_ptr<Expr>& base, unsigned int width) :Expr(width,0)
	{
		rightExpr = base;
	}

	virtual ExprKind getKind() {
		return ExprKind::ZEXT;
	}
};