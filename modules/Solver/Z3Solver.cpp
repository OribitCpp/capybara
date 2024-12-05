#include "Z3Solver.h"
#include "runtime/ArrayObject.h"

Z3Solver::Z3Solver()
{
}

Z3Solver::~Z3Solver()
{
}

bool Z3Solver::computeTruth(const std::vector<std::shared_ptr<Expr>>& contrains, std::shared_ptr<Expr> expr, bool result)
{
    bool hasSolution = false; // to remove compiler warning
    //bool status = internalRunSolver(query, /*objects=*/NULL, /*values=*/NULL, hasSolution);
    result = !hasSolution;
    //return status;
    return false;
}

bool Z3Solver::computeValue(const std::vector<std::shared_ptr<Expr>>& contrains, std::shared_ptr<Expr> expr, std::shared_ptr<ConstantExpr>& result)
{
    std::vector<std::shared_ptr<ArrayObject>> objects;
    std::vector<std::vector<unsigned char> > values;
    bool hasSolution;

    // Find the object used in the expression, and compute an assignment
    // for them.
    //findSymbolicObjects(expr, objects);
    //if (!computeInitialValues(query.withFalse(), objects, values, hasSolution))
    //    return false;
    //assert(hasSolution && "state has invalid constraint set");

    //// Evaluate the expression with the computed assignment.
    //Assignment a(objects, values);
    //result = a.evaluate(query.expr);

    return true;
}
