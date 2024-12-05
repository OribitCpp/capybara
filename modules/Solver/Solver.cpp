#include "Solver.h"
#include "expr/ConstantExpr.h"

#include <cassert>

bool Solver::evaluate(const std::vector<std::shared_ptr<Expr>> constaints, std::shared_ptr<Expr> expr, SolverValidity& result)
{
    assert(expr->getWidth() == 1 && "Invalid expression type!");

    // Maintain invariants implementations expect.
    if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(expr)) {
        result = CE->isTrue() ? True : False;
        return true;
    }

    return computeValidity(constaints, expr, result);
}

bool Solver::mustBeTrue(const std::vector<std::shared_ptr<Expr>> constaints, std::shared_ptr<Expr> expr, bool& result)
{
    assert(expr->getWidth() == 1 && "Invalid expression type!");

    if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(expr)) {
        result = CE->isTrue() ? true : false;
        return true;
    }
    return computeTruth(constaints,expr, result);
}

bool Solver::getValue(const std::vector<std::shared_ptr<Expr>> constaints, std::shared_ptr<Expr> expr, std::shared_ptr<ConstantExpr>& result)
{
    if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(expr)) {
        result = CE;
        return true;
    }

    // FIXME: Push ConstantExpr requirement down.
    std::shared_ptr<ConstantExpr> tmp;
    if (!computeValue(constaints, expr, tmp))
        return false;

    result = std::dynamic_pointer_cast<ConstantExpr>(tmp);
    return true;
}

bool Solver::computeValidity(const std::vector<std::shared_ptr<Expr>>& contrains, std::shared_ptr<Expr> expr, SolverValidity &result)
{
    bool isTrue, isFalse;
    if (!computeTruth(contrains,expr , isTrue))
        return false;
    if (isTrue) {
        result = SolverValidity::True;
    }
    else {
        if (!computeTruth(contrains,Expr::createIsZero(expr), isFalse))
            return false;
        result = isFalse ? SolverValidity::False : SolverValidity::Unknown;
    }
    return true;
}
