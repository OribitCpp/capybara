#ifndef  EXTRACTEXPR_H
#define EXTRACTEXPR_H

#include "Expr.h"

class ExtractExpr : public Expr {
public:
	ExtractExpr(const std::shared_ptr<Expr>&baseExpr, uint64_t start, uint64_t offset);
	~ExtractExpr();
	virtual ExprKind getKind() override;
private:
	std::shared_ptr<Expr> m_baseExpr;
};

#endif // ! ExtractExpr_h
