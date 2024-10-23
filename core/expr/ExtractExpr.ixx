export module ConcatExpr;

import Expr;

export  class ExtractExpr : public Expr {
public:
	ExtractExpr(const std::shared_ptr<Expr>&baseExpr, unsigned int start, unsigned int offset):Expr(start, offset)
	{
		rightExpr = baseExpr;
	}
	~ExtractExpr(){}
	virtual ExprKind getKind() override
	{
		return ExprKind::EXTRACT;
	}
private:
};

