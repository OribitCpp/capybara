#ifndef  EXTRACTEXPR_H
#define EXTRACTEXPR_H

#include "Expr.h"

class ExtractExpr : public Expr {
public:
	ExtractExpr(const std::shared_ptr<Expr>&baseExpr, uint64_t start, uint64_t offset);
	~ExtractExpr();
	virtual ExprKind getKind() override;
	uint64_t offset() const { return m_offset; }
	uint64_t start() const { return m_start; }
private:
	std::shared_ptr<Expr> m_baseExpr;
	uint64_t m_offset;
	uint64_t m_start;
};

#endif // ! ExtractExpr_h
