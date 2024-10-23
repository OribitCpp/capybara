export module SelectExpr;

import Expr;
export class SelectExpr: public Expr{
public:
	SelectExpr(const std::shared_ptr<Expr> &cond, const std::shared_ptr<Expr> &trueExpr,const std::shared_ptr<Expr> &falseExpr):Expr(0,0)
	{
		condition = cond;
		leftExpr = trueExpr;
		rightExpr = falseExpr;
	}

	ExprKind getKind() override
	{
		return ExprKind::SELECT;
	}
private:
	std::shared_ptr<Expr> condition;
};
