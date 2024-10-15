#ifndef  EXTRACTEXPR_H
#define EXTRACTEXPR_H

#include "Expr.h"

class ExtractExpr : public Expr {
public:
	ExtractExpr(const std::shared_ptr<Expr>&baseExpr, unsigned int start, unsigned int offset);
	~ExtractExpr();
	virtual ExprKind getKind() override;
private:
	std::shared_ptr<Expr> m_baseExpr;
};

#endif // ! ExtractExpr_h
