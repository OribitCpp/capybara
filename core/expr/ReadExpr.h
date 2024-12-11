#ifndef  READEXPR_H
#define READEXPR_H

#include "expr/Expr.h"
#include "UpdateNode.h"

class ReadExpr : public Expr{
public:
	ReadExpr(const UpdateList& updates, std::shared_ptr<Expr>& index);
	virtual ExprKind getKind() override;
	UpdateList updates;
};

#endif // ! READEXPR_H
