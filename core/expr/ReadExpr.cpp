#include "ReadExpr.h"

ReadExpr::ReadExpr(const UpdateList& updates, std::shared_ptr<Expr>& index):Expr(0,0),updates(updates) {
	leftExpr = index;
}
ExprKind ReadExpr::getKind() {
	return  ExprKind::READ;
}