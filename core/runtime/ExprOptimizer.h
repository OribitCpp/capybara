#ifndef EXPROPTIMIZER_H
#define EXPROPTIMIZER_H

#include "expr/Expr.h"
#include <unordered_map>
#include <vector>

class ExprOptimizer {

public:
	std::shared_ptr<Expr> optimizeExpr(const std::shared_ptr<Expr>& expr, bool valueOnly);
private:
	std::unordered_map<std::shared_ptr<Expr>,std::vector<Expr>> m_cachedExprOptimized;
};

#endif // !ExprOptimizer_h
