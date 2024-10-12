#ifndef  CONSTANTEXPR_H
#define CONSTANTEXPR_H

#include "Expr.h"
#include <llvm/ADT/APInt.h>

class ConstantExpr:public Expr {
public:
	ConstantExpr();

private:
};
#endif // ! ConstantExpr_h
