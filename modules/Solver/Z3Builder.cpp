#include "Z3Builder.h"
#include "Logger.h"
#include "statistic/StatisticManager.h"
#include <assert.h>
#include <cstdint>

#include "expr/ConstantExpr.h"
#include "expr/NotOptimizedExpr.h"
#include "expr/ReadExpr.h"
#include "expr/SelectExpr.h"
#include "expr/ConcatExpr.h"
#include "expr/ExtractExpr.h"
#include "expr/Bit/AndExpr.h"
#include "expr/Bit/AShrExpr.h"
#include "expr/Bit/LShrExpr.h"
#include "expr/Bit/NotExpr.h"
#include "expr/Bit/OrExpr.h"
#include "expr/Bit/ShlExpr.h"
#include "expr/Bit/XorExpr.h"
#include "expr/Cast/SExtExpr.h"
#include "expr/Cast/ZExtExpr.h"
#include "expr/Arithmetic/AddExpr.h"
#include "expr/Arithmetic/MulExpr.h"
#include "expr/Arithmetic/SDivExpr.h"
#include "expr/Arithmetic/SRemExpr.h"
#include "expr/Arithmetic/SubExpr.h"
#include "expr/Arithmetic/UDivExpr.h"
#include "expr/Arithmetic/URemExpr.h"
#include "expr/Compare/EqExpr.h"
#include "expr/Compare/NeExpr.h"
#include "expr/Compare/SgeExpr.h"
#include "expr/Compare/SgtExpr.h"
#include "expr/Compare/SleExpr.h"
#include "expr/Compare/SltExpr.h"
#include "expr/Compare/UgeExpr.h"
#include "expr/Compare/UgtExpr.h"
#include "expr/Compare/UleExpr.h"
#include "expr/Compare/UltExpr.h"

// Declared here rather than `Z3Builder.h` so they can be called in gdb.
template <> void Z3NodeHandle<Z3_sort>::dump() {
	Logger::error("Z3SortHandle: {}", Z3_sort_to_string(context, node));
}
template <> void Z3NodeHandle<Z3_ast>::dump() {
	Logger::error("Z3ASTHandle: {}", Z3_ast_to_string(context, as_ast()));
}

void custom_z3_error_handler(Z3_context ctx, Z3_error_code ec) {
    ::Z3_string errorMsg = Z3_get_error_msg(ctx, ec);
    // FIXME: This is kind of a hack. The value comes from the enum
    // Z3_CANCELED_MSG but this isn't currently exposed by Z3's C API
    if (strcmp(errorMsg, "canceled") == 0) {
        // Solver timeout is not a fatal error
        return;
    }
    llvm::errs() << "Error: Incorrect use of Z3. [" << ec << "] " << errorMsg
        << "\n";
    // NOTE: The current implementation of `Z3_close_log()` can be safely
    // called even if the log isn't open.
    Z3_close_log();
    abort();
}

Z3Builder::Z3Builder(): autoClearConstructCache(true){
    Z3_open_log("z3_solver.log");
    // FIXME: Should probably let the client pass in a Z3_config instead
    Z3_config cfg = Z3_mk_config();
    // It is very important that we ask Z3 to let us manage memory so that
    // we are able to cache expressions and sorts.
    m_z3Context = Z3_mk_context_rc(cfg);
    // Make sure we handle any errors reported by Z3.
    Z3_set_error_handler(m_z3Context, custom_z3_error_handler);
    // When emitting Z3 expressions make them SMT-LIBv2 compliant
    Z3_set_ast_print_mode(m_z3Context, Z3_PRINT_SMTLIB2_COMPLIANT);
    Z3_del_config(cfg);
}

Z3Builder::~Z3Builder() {
    // Clear caches so exprs/sorts gets freed before the destroying context
    // they aren associated with.
    clearConstructCache();
    //_arr_hash.clear();
    constant_array_assertions.clear();
    Z3_del_context(m_z3Context);
    Z3_close_log();
}

Z3SortHandle Z3Builder::getBvSort(unsigned width) {
    // FIXME: cache these
    return Z3SortHandle(Z3_mk_bv_sort(m_z3Context, width), m_z3Context);
}

Z3SortHandle Z3Builder::getArraySort(Z3SortHandle domainSort,
    Z3SortHandle rangeSort) {
    // FIXME: cache these
    return Z3SortHandle(Z3_mk_array_sort(m_z3Context, domainSort, rangeSort), m_z3Context);
}

Z3ASTHandle Z3Builder::buildArray(const char* name, unsigned indexWidth,
    unsigned valueWidth) {
    Z3SortHandle domainSort = getBvSort(indexWidth);
    Z3SortHandle rangeSort = getBvSort(valueWidth);
    Z3SortHandle t = getArraySort(domainSort, rangeSort);
    Z3_symbol s = Z3_mk_string_symbol(m_z3Context, const_cast<char*>(name));
    return Z3ASTHandle(Z3_mk_const(m_z3Context, s, t), m_z3Context);
}

Z3ASTHandle Z3Builder::getTrue() { return Z3ASTHandle(Z3_mk_true(m_z3Context), m_z3Context); }

Z3ASTHandle Z3Builder::getFalse() { return Z3ASTHandle(Z3_mk_false(m_z3Context), m_z3Context); }

Z3ASTHandle Z3Builder::bvOne(unsigned width) { return bvZExtConst(width, 1); }

Z3ASTHandle Z3Builder::bvZero(unsigned width) { return bvZExtConst(width, 0); }

Z3ASTHandle Z3Builder::bvMinusOne(unsigned width) {
    return bvSExtConst(width, (int64_t)-1);
}

Z3ASTHandle Z3Builder::bvConst32(unsigned width, uint32_t value) {
    Z3SortHandle t = getBvSort(width);
    return Z3ASTHandle(Z3_mk_unsigned_int(m_z3Context, value, t), m_z3Context);
}

Z3ASTHandle Z3Builder::bvConst64(unsigned width, uint64_t value) {
    Z3SortHandle t = getBvSort(width);
    return Z3ASTHandle(Z3_mk_unsigned_int64(m_z3Context, value, t), m_z3Context);
}

Z3ASTHandle Z3Builder::bvZExtConst(unsigned width, uint64_t value) {
    if (width <= 64)
        return bvConst64(width, value);

    Z3ASTHandle expr = Z3ASTHandle(bvConst64(64, value), m_z3Context);
    Z3ASTHandle zero = Z3ASTHandle(bvConst64(64, 0), m_z3Context);
    for (width -= 64; width > 64; width -= 64)
        expr = Z3ASTHandle(Z3_mk_concat(m_z3Context, zero, expr), m_z3Context);
    return Z3ASTHandle(Z3_mk_concat(m_z3Context, bvConst64(width, 0), expr), m_z3Context);
}

Z3ASTHandle Z3Builder::bvSExtConst(unsigned width, uint64_t value) {
    if (width <= 64)
        return bvConst64(width, value);

    Z3SortHandle t = getBvSort(width - 64);
    if (value >> 63) {
        Z3ASTHandle r = Z3ASTHandle(Z3_mk_int64(m_z3Context, -1, t), m_z3Context);
        return Z3ASTHandle(Z3_mk_concat(m_z3Context, r, bvConst64(64, value)), m_z3Context);
    }

    Z3ASTHandle r = Z3ASTHandle(Z3_mk_int64(m_z3Context, 0, t), m_z3Context);
    return Z3ASTHandle(Z3_mk_concat(m_z3Context, r, bvConst64(64, value)), m_z3Context);
}

Z3ASTHandle Z3Builder::bvBoolExtract(Z3ASTHandle expr, int bit) {
    return Z3ASTHandle(Z3_mk_eq(m_z3Context, bvExtract(expr, bit, bit), bvOne(1)), m_z3Context);
}

Z3ASTHandle Z3Builder::bvExtract(Z3ASTHandle expr, unsigned top,
    unsigned bottom) {
    return Z3ASTHandle(Z3_mk_extract(m_z3Context, top, bottom, expr), m_z3Context);
}

Z3ASTHandle Z3Builder::eqExpr(Z3ASTHandle a, Z3ASTHandle b) {
    return Z3ASTHandle(Z3_mk_eq(m_z3Context, a, b), m_z3Context);
}

// logical right shift
Z3ASTHandle Z3Builder::bvRightShift(Z3ASTHandle expr, unsigned shift) {
    unsigned width = getBVLength(expr);

    if (shift == 0) {
        return expr;
    }
    else if (shift >= width) {
        return bvZero(width); // Overshift to zero
    }
    else {
        return Z3ASTHandle(
            Z3_mk_concat(m_z3Context, bvZero(shift), bvExtract(expr, width - 1, shift)),
            m_z3Context);
    }
}

// logical left shift
Z3ASTHandle Z3Builder::bvLeftShift(Z3ASTHandle expr, unsigned shift) {
    unsigned width = getBVLength(expr);

    if (shift == 0) {
        return expr;
    }
    else if (shift >= width) {
        return bvZero(width); // Overshift to zero
    }
    else {
        return Z3ASTHandle(
            Z3_mk_concat(m_z3Context, bvExtract(expr, width - shift - 1, 0), bvZero(shift)),
            m_z3Context);
    }
}

// left shift by a variable amount on an expression of the specified width
Z3ASTHandle Z3Builder::bvVarLeftShift(Z3ASTHandle expr, Z3ASTHandle shift) {
    unsigned width = getBVLength(expr);
    Z3ASTHandle res = bvZero(width);

    // construct a big if-then-elif-elif-... with one case per possible shift
    // amount
    for (int i = width - 1; i >= 0; i--) {
        res =
            iteExpr(eqExpr(shift, bvConst32(width, i)), bvLeftShift(expr, i), res);
    }

    // If overshifting, shift to zero
    Z3ASTHandle ex = bvLtExpr(shift, bvConst32(getBVLength(shift), width));
    res = iteExpr(ex, res, bvZero(width));
    return res;
}

// logical right shift by a variable amount on an expression of the specified
// width
Z3ASTHandle Z3Builder::bvVarRightShift(Z3ASTHandle expr, Z3ASTHandle shift) {
    unsigned width = getBVLength(expr);
    Z3ASTHandle res = bvZero(width);

    // construct a big if-then-elif-elif-... with one case per possible shift
    // amount
    for (int i = width - 1; i >= 0; i--) {
        res =
            iteExpr(eqExpr(shift, bvConst32(width, i)), bvRightShift(expr, i), res);
    }

    // If overshifting, shift to zero
    Z3ASTHandle ex = bvLtExpr(shift, bvConst32(getBVLength(shift), width));
    res = iteExpr(ex, res, bvZero(width));
    return res;
}

// arithmetic right shift by a variable amount on an expression of the specified
// width
Z3ASTHandle Z3Builder::bvVarArithRightShift(Z3ASTHandle expr,
    Z3ASTHandle shift) {
    unsigned width = getBVLength(expr);

    // get the sign bit to fill with
    Z3ASTHandle signedBool = bvBoolExtract(expr, width - 1);

    // start with the result if shifting by width-1
    Z3ASTHandle res = constructAShrByConstant(expr, width - 1, signedBool);

    // construct a big if-then-elif-elif-... with one case per possible shift
    // amount
    // XXX more efficient to move the ite on the sign outside all exprs?
    // XXX more efficient to sign extend, right shift, then extract lower bits?
    for (int i = width - 2; i >= 0; i--) {
        res = iteExpr(eqExpr(shift, bvConst32(width, i)),
            constructAShrByConstant(expr, i, signedBool), res);
    }

    // If overshifting, shift to zero
    Z3ASTHandle ex = bvLtExpr(shift, bvConst32(getBVLength(shift), width));
    res = iteExpr(ex, res, bvZero(width));
    return res;
}

Z3ASTHandle Z3Builder::notExpr(Z3ASTHandle expr) {
    return Z3ASTHandle(Z3_mk_not(m_z3Context, expr), m_z3Context);
}

Z3ASTHandle Z3Builder::bvNotExpr(Z3ASTHandle expr) {
    return Z3ASTHandle(Z3_mk_bvnot(m_z3Context, expr), m_z3Context);
}

Z3ASTHandle Z3Builder::andExpr(Z3ASTHandle lhs, Z3ASTHandle rhs) {
    ::Z3_ast args[2] = { lhs, rhs };
    return Z3ASTHandle(Z3_mk_and(m_z3Context, 2, args), m_z3Context);
}

Z3ASTHandle Z3Builder::bvAndExpr(Z3ASTHandle lhs, Z3ASTHandle rhs) {
    return Z3ASTHandle(Z3_mk_bvand(m_z3Context, lhs, rhs), m_z3Context);
}

Z3ASTHandle Z3Builder::orExpr(Z3ASTHandle lhs, Z3ASTHandle rhs) {
    ::Z3_ast args[2] = { lhs, rhs };
    return Z3ASTHandle(Z3_mk_or(m_z3Context, 2, args), m_z3Context);
}

Z3ASTHandle Z3Builder::bvOrExpr(Z3ASTHandle lhs, Z3ASTHandle rhs) {
    return Z3ASTHandle(Z3_mk_bvor(m_z3Context, lhs, rhs), m_z3Context);
}

Z3ASTHandle Z3Builder::iffExpr(Z3ASTHandle lhs, Z3ASTHandle rhs) {
    Z3SortHandle lhsSort = Z3SortHandle(Z3_get_sort(m_z3Context, lhs), m_z3Context);
    Z3SortHandle rhsSort = Z3SortHandle(Z3_get_sort(m_z3Context, rhs), m_z3Context);
    assert(Z3_get_sort_kind(m_z3Context, lhsSort) == Z3_get_sort_kind(m_z3Context, rhsSort) &&
        "lhs and rhs sorts must match");
    assert(Z3_get_sort_kind(m_z3Context, lhsSort) == Z3_BOOL_SORT && "args must have BOOL sort");
    return Z3ASTHandle(Z3_mk_iff(m_z3Context, lhs, rhs), m_z3Context);
}

Z3ASTHandle Z3Builder::bvXorExpr(Z3ASTHandle lhs, Z3ASTHandle rhs) {
    return Z3ASTHandle(Z3_mk_bvxor(m_z3Context, lhs, rhs), m_z3Context);
}

Z3ASTHandle Z3Builder::bvSignExtend(Z3ASTHandle src, unsigned width) {
    unsigned src_width =
        Z3_get_bv_sort_size(m_z3Context, Z3SortHandle(Z3_get_sort(m_z3Context, src), m_z3Context));
    assert(src_width <= width && "attempted to extend longer data");

    return Z3ASTHandle(Z3_mk_sign_ext(m_z3Context, width - src_width, src), m_z3Context);
}

Z3ASTHandle Z3Builder::writeExpr(Z3ASTHandle array, Z3ASTHandle index,
    Z3ASTHandle value) {
    return Z3ASTHandle(Z3_mk_store(m_z3Context, array, index, value), m_z3Context);
}

Z3ASTHandle Z3Builder::readExpr(Z3ASTHandle array, Z3ASTHandle index) {
    return Z3ASTHandle(Z3_mk_select(m_z3Context, array, index), m_z3Context);
}

Z3ASTHandle Z3Builder::iteExpr(Z3ASTHandle condition, Z3ASTHandle whenTrue,
    Z3ASTHandle whenFalse) {
    return Z3ASTHandle(Z3_mk_ite(m_z3Context, condition, whenTrue, whenFalse), m_z3Context);
}

unsigned Z3Builder::getBVLength(Z3ASTHandle expr) {
    return Z3_get_bv_sort_size(m_z3Context, Z3SortHandle(Z3_get_sort(m_z3Context, expr), m_z3Context));
}

Z3ASTHandle Z3Builder::bvLtExpr(Z3ASTHandle lhs, Z3ASTHandle rhs) {
    return Z3ASTHandle(Z3_mk_bvult(m_z3Context, lhs, rhs), m_z3Context);
}

Z3ASTHandle Z3Builder::bvLeExpr(Z3ASTHandle lhs, Z3ASTHandle rhs) {
    return Z3ASTHandle(Z3_mk_bvule(m_z3Context, lhs, rhs), m_z3Context);
}

Z3ASTHandle Z3Builder::sbvLtExpr(Z3ASTHandle lhs, Z3ASTHandle rhs) {
    return Z3ASTHandle(Z3_mk_bvslt(m_z3Context, lhs, rhs), m_z3Context);
}

Z3ASTHandle Z3Builder::sbvLeExpr(Z3ASTHandle lhs, Z3ASTHandle rhs) {
    return Z3ASTHandle(Z3_mk_bvsle(m_z3Context, lhs, rhs), m_z3Context);
}

Z3ASTHandle Z3Builder::constructAShrByConstant(Z3ASTHandle expr, unsigned shift,
    Z3ASTHandle isSigned) {
    unsigned width = getBVLength(expr);

    if (shift == 0) {
        return expr;
    }
    else if (shift >= width) {
        return bvZero(width); // Overshift to zero
    }
    else {
        // FIXME: Is this really the best way to interact with Z3?
        return iteExpr(isSigned,
            Z3ASTHandle(Z3_mk_concat(m_z3Context, bvMinusOne(shift),
                bvExtract(expr, width - 1, shift)),
                m_z3Context),
            bvRightShift(expr, shift));
    }
}

Z3ASTHandle Z3Builder::getInitialArray(const std::shared_ptr<ArrayObject>&root) {

    assert(root);
    Z3ASTHandle array_expr;
    //bool hashed = _arr_hash.lookupArrayExpr(root, array_expr);
    bool hashed = false;

    if (!hashed) {
        // Unique arrays by name, so we make sure the name is unique by
        // using the size of the array hash as a counter.
        std::string unique_id = std::to_string(m_arr_hash.m_array_hash.size());
        // Prefix unique ID with '_' to avoid name collision if name ends with
        // number
        std::string unique_name = root->getName() + "_" + unique_id;

        array_expr = buildArray(unique_name.c_str(), root->domain(),  root->range());

        const std::vector<std::shared_ptr<ConstantExpr>>& arrayValues = root->getValues();
        if (arrayValues.empty() == false && constant_array_assertions.count(root) == 0) {
            std::vector<Z3ASTHandle> array_assertions;
            for (unsigned i = 0, e = arrayValues.size(); i != e; ++i) {
                // construct(= (select i root) root->value[i]) to be asserted in
                // Z3Solver.cpp
                int width_out;
                Z3ASTHandle array_value =  construct(arrayValues[i], &width_out);
                assert(width_out == (int)root->range() && "Value doesn't match root range");
                array_assertions.push_back(
                    eqExpr(readExpr(array_expr, bvConst32(root->domain(), i)),
                        array_value));
            }
            constant_array_assertions[root] = std::move(array_assertions);
        }

        m_arr_hash.hashArrayExpr(root, array_expr);
    }

    return (array_expr);
}

Z3ASTHandle Z3Builder::getInitialRead(const std::shared_ptr<ArrayObject> &root, unsigned index) {
    return readExpr(getInitialArray(root), bvConst32(32, index));
}

Z3ASTHandle Z3Builder::getArrayForUpdate(const  std::shared_ptr<ArrayObject> root, std::shared_ptr < UpdateNode> un) {
    // Iterate over the update nodes, until we find a cached version of the node,
    // or no more update nodes remain
    Z3ASTHandle un_expr;
    std::vector<std::shared_ptr<UpdateNode>> update_nodes;
    for (; un && !m_arr_hash.lookupUpdateNodeExpr(un, un_expr);  un = un->next) {
        update_nodes.push_back(un);
    }
    if (!un) {
        un_expr = getInitialArray(root);
    }
    // `un_expr` now holds an expression for the array - either from cache or by
    // virtue of being the initial array expression

    // Create and cache solver expressions based on the update nodes starting from
    // the oldest
    for (const auto& un :  llvm::make_range(update_nodes.crbegin(), update_nodes.crend())) {
        un_expr = writeExpr(un_expr, construct(un->index, 0), construct(un->value, 0));
       m_arr_hash.hashUpdateNodeExpr(un, un_expr);
    }
    return un_expr;
}

/** if *width_out!=1 then result is a bitvector,
    otherwise it is a bool */
Z3ASTHandle Z3Builder::construct(std::shared_ptr<Expr> e, int* width_out) {
    // TODO: We could potentially use Z3_simplify() here
    // to store simpler expressions.
    if (std::dynamic_pointer_cast<ConstantExpr>(e)) {
        return constructActual(e, width_out);
    }
    else {
        std::unordered_map<std::shared_ptr<Expr>, std::pair<Z3ASTHandle, unsigned> >::iterator it =  m_constructed.find(e);
        if (it != m_constructed.end()) {
            if (width_out) *width_out = it->second.second;
            return it->second.first;
        }
        else {
            int width;
            if (!width_out)
                width_out = &width;
            Z3ASTHandle res = constructActual(e, width_out);
            m_constructed.insert(std::make_pair(e, std::make_pair(res, *width_out)));
            return res;
        }
    }
}

/** if *width_out!=1 then result is a bitvector,
    otherwise it is a bool */
Z3ASTHandle Z3Builder::constructActual(std::shared_ptr<Expr> e, int* width_out) {
    int width;
    if (!width_out)
        width_out = &width;

    ++StatisticManager::queryConstructs;

    switch (e->getKind()) {
    case ExprKind::CONSTANT: {
        std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(e);
        *width_out = CE->getWidth();

        // Coerce to bool if necessary.
        if (*width_out == 1)
            return CE->isTrue() ? getTrue() : getFalse();

        // Fast path.
        if (*width_out <= 32)
            return bvConst32(*width_out, CE->getZExtValue(32));
        if (*width_out <= 64)
            return bvConst64(*width_out, CE->getZExtValue());

        std::shared_ptr<ConstantExpr> Tmp = CE;
        Z3ASTHandle Res = bvConst64(64, Tmp->Extract(0, 64)->getZExtValue());
        while (Tmp->getWidth() > 64) {
            Tmp = Tmp->Extract(64, Tmp->getWidth() - 64);
            unsigned Width = std::min(64ULL, Tmp->getWidth());
            Res = Z3ASTHandle(
                Z3_mk_concat(m_z3Context,
                    bvConst64(Width, Tmp->Extract(0, Width)->getZExtValue()),
                    Res),
                m_z3Context);
        }
        return Res;
    }

                       // Special
    case ExprKind::NOTOPTIMIZED: {
        std::shared_ptr<NotOptimizedExpr> noe = std::dynamic_pointer_cast<NotOptimizedExpr>(e);
        return construct(noe->leftExpr, width_out);
    }

    case ExprKind::READ: {
        std::shared_ptr < ReadExpr> re = std::dynamic_pointer_cast<ReadExpr>(e);
        assert(re && re->updates.root);
        *width_out = re->updates.root->range();
        return readExpr(getArrayForUpdate(re->updates.root, re->updates.head), construct(re->leftExpr, 0));
    }

    case ExprKind::SELECT: {
        std::shared_ptr<SelectExpr> se = std::dynamic_pointer_cast<SelectExpr>(e);
        Z3ASTHandle cond = construct(se->condition, 0);
        Z3ASTHandle tExpr = construct(se->leftExpr, width_out);
        Z3ASTHandle fExpr = construct(se->rightExpr, width_out);
        return iteExpr(cond, tExpr, fExpr);
    }

    case ExprKind::CONCAT: {
        std::shared_ptr<ConcatExpr> ce = std::dynamic_pointer_cast<ConcatExpr>(e);
        Z3ASTHandle res = construct(ce->rightExpr, 0);
        res = Z3ASTHandle(Z3_mk_concat(m_z3Context, construct(ce->leftExpr, 0), res), m_z3Context);
        *width_out = ce->getWidth();
        return res;
    }

    case ExprKind::EXTRACT: {
        std::shared_ptr<ExtractExpr> ee = std::dynamic_pointer_cast<ExtractExpr>(e);
        Z3ASTHandle src = construct(ee->leftExpr, width_out);
        *width_out = ee->getWidth();
        if (*width_out == 1) {
            return bvBoolExtract(src, ee->offset());
        }
        else {
            return bvExtract(src, ee->offset() + *width_out - 1, ee->offset());
        }
    }

    case ExprKind::ZEXT: {
        int srcWidth;
        std::shared_ptr<ZExtExpr> ce = std::dynamic_pointer_cast<ZExtExpr>(e);
        Z3ASTHandle src = construct(ce->leftExpr, &srcWidth);
        *width_out = ce->getWidth();
        if (srcWidth == 1) {
            return iteExpr(src, bvOne(*width_out), bvZero(*width_out));
        }
        else {
            assert(*width_out > srcWidth && "Invalid width_out");
            return Z3ASTHandle(Z3_mk_concat(m_z3Context, bvZero(*width_out - srcWidth), src),
                m_z3Context);
        }
    }

    case ExprKind::SEXT: {
        int srcWidth;
        std::shared_ptr<SExtExpr> ce = std::dynamic_pointer_cast<SExtExpr>(e);
        Z3ASTHandle src = construct(ce->leftExpr, &srcWidth);
        *width_out = ce->getWidth();
        if (srcWidth == 1) {
            return iteExpr(src, bvMinusOne(*width_out), bvZero(*width_out));
        }
        else {
            return bvSignExtend(src, *width_out);
        }
    }

                   // Arithmetic
    case ExprKind::ADD: {
        std::shared_ptr<AddExpr> ae = std::dynamic_pointer_cast<AddExpr>(e);
        Z3ASTHandle left = construct(ae->leftExpr, width_out);
        Z3ASTHandle right = construct(ae->rightExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized add");
        Z3ASTHandle result = Z3ASTHandle(Z3_mk_bvadd(m_z3Context, left, right), m_z3Context);
        assert(getBVLength(result) == static_cast<unsigned>(*width_out) &&
            "width mismatch");
        return result;
    }

    case ExprKind::SUB: {
        std::shared_ptr<SubExpr> se = std::dynamic_pointer_cast<SubExpr>(e);
        Z3ASTHandle left = construct(se->leftExpr, width_out);
        Z3ASTHandle right = construct(se->rightExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized sub");
        Z3ASTHandle result = Z3ASTHandle(Z3_mk_bvsub(m_z3Context, left, right), m_z3Context);
        assert(getBVLength(result) == static_cast<unsigned>(*width_out) &&
            "width mismatch");
        return result;
    }

    case ExprKind::MUL: {
        std::shared_ptr<MulExpr> me = std::dynamic_pointer_cast<MulExpr>(e);
        Z3ASTHandle right = construct(me->rightExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized mul");
        Z3ASTHandle left = construct(me->leftExpr, width_out);
        Z3ASTHandle result = Z3ASTHandle(Z3_mk_bvmul(m_z3Context, left, right), m_z3Context);
        assert(getBVLength(result) == static_cast<unsigned>(*width_out) &&
            "width mismatch");
        return result;
    }

    case ExprKind::UDIV: {
        std::shared_ptr<UDivExpr> de = std::dynamic_pointer_cast<UDivExpr>(e);
        Z3ASTHandle left = construct(de->leftExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized udiv");

        if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(de->rightExpr)) {
            if (CE->getWidth() <= 64) {
                uint64_t divisor = CE->getZExtValue();
                if (isPowerOfTwo(divisor))
                    return bvRightShift(left, indexOfSingleBit(divisor));
            }
        }

        Z3ASTHandle right = construct(de->rightExpr, width_out);
        Z3ASTHandle result = Z3ASTHandle(Z3_mk_bvudiv(m_z3Context, left, right), m_z3Context);
        assert(getBVLength(result) == static_cast<unsigned>(*width_out) &&
            "width mismatch");
        return result;
    }

    case ExprKind::SDIV: {
        std::shared_ptr<SDivExpr> de = std::dynamic_pointer_cast<SDivExpr>(e);
        Z3ASTHandle left = construct(de->leftExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized sdiv");
        Z3ASTHandle right = construct(de->rightExpr, width_out);
        Z3ASTHandle result = Z3ASTHandle(Z3_mk_bvsdiv(m_z3Context, left, right), m_z3Context);
        assert(getBVLength(result) == static_cast<unsigned>(*width_out) &&
            "width mismatch");
        return result;
    }

    case ExprKind::UREM: {
        std::shared_ptr<URemExpr> de = std::dynamic_pointer_cast<URemExpr>(e);
        Z3ASTHandle left = construct(de->leftExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized urem");

        if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(de->rightExpr)) {
            if (CE->getWidth() <= 64) {
                uint64_t divisor = CE->getZExtValue();

                if (isPowerOfTwo(divisor)) {
                    // FIXME: This should be unsigned but currently needs to be signed to
                    // avoid signed-unsigned comparison in assert.
                    int bits = indexOfSingleBit(divisor);

                    // special case for modding by 1 or else we bvExtract -1:0
                    if (bits == 0) {
                        return bvZero(*width_out);
                    }
                    else {
                        assert(*width_out > bits && "invalid width_out");
                        return Z3ASTHandle(Z3_mk_concat(m_z3Context, bvZero(*width_out - bits),
                            bvExtract(left, bits - 1, 0)),
                            m_z3Context);
                    }
                }
            }
        }

        Z3ASTHandle right = construct(de->rightExpr, width_out);
        Z3ASTHandle result = Z3ASTHandle(Z3_mk_bvurem(m_z3Context, left, right), m_z3Context);
        assert(getBVLength(result) == static_cast<unsigned>(*width_out) &&
            "width mismatch");
        return result;
    }

    case ExprKind::SREM: {
        std::shared_ptr<SRemExpr> de = std::dynamic_pointer_cast<SRemExpr>(e);
        Z3ASTHandle left = construct(de->leftExpr, width_out);
        Z3ASTHandle right = construct(de->rightExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized srem");
        // LLVM's srem instruction says that the sign follows the dividend
        // (``left``).
        // Z3's C API says ``Z3_mk_bvsrem()`` does this so these seem to match.
        Z3ASTHandle result = Z3ASTHandle(Z3_mk_bvsrem(m_z3Context, left, right), m_z3Context);
        assert(getBVLength(result) == static_cast<unsigned>(*width_out) &&
            "width mismatch");
        return result;
    }

                   // Bitwise
    case ExprKind::NOT: {
        std::shared_ptr<NotExpr> ne = std::dynamic_pointer_cast<NotExpr>(e);
        Z3ASTHandle expr = construct(ne->leftExpr, width_out);
        if (*width_out == 1) {
            return notExpr(expr);
        }
        else {
            return bvNotExpr(expr);
        }
    }

    case ExprKind::AND: {
        std::shared_ptr<AndExpr> ae = std::dynamic_pointer_cast<AndExpr>(e);
        Z3ASTHandle left = construct(ae->leftExpr, width_out);
        Z3ASTHandle right = construct(ae->rightExpr, width_out);
        if (*width_out == 1) {
            return andExpr(left, right);
        }
        else {
            return bvAndExpr(left, right);
        }
    }

    case ExprKind::OR: {
        std::shared_ptr<OrExpr> oe = std::dynamic_pointer_cast<OrExpr>(e);
        Z3ASTHandle left = construct(oe->leftExpr, width_out);
        Z3ASTHandle right = construct(oe->rightExpr, width_out);
        if (*width_out == 1) {
            return orExpr(left, right);
        }
        else {
            return bvOrExpr(left, right);
        }
    }

    case ExprKind::XOR: {
        std::shared_ptr<XorExpr> xe = std::dynamic_pointer_cast<XorExpr>(e);
        Z3ASTHandle left = construct(xe->leftExpr, width_out);
        Z3ASTHandle right = construct(xe->rightExpr, width_out);

        if (*width_out == 1) {
            // XXX check for most efficient?
            return iteExpr(left, Z3ASTHandle(notExpr(right)), right);
        }
        else {
            return bvXorExpr(left, right);
        }
    }

    case ExprKind::SHL: {
        std::shared_ptr<ShlExpr> se = std::dynamic_pointer_cast<ShlExpr>(e);
        Z3ASTHandle left = construct(se->leftExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized shl");

        if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(se->rightExpr)) {
            return bvLeftShift(left, (unsigned)CE->getLimitedValue());
        }
        else {
            int shiftWidth;
            Z3ASTHandle amount = construct(se->rightExpr, &shiftWidth);
            return bvVarLeftShift(left, amount);
        }
    }

    case ExprKind::LSHR: {
        std::shared_ptr<LShrExpr> lse = std::dynamic_pointer_cast<LShrExpr>(e);
        Z3ASTHandle left = construct(lse->leftExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized lshr");

        if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(lse->rightExpr)) {
            return bvRightShift(left, (unsigned)CE->getLimitedValue());
        }
        else {
            int shiftWidth;
            Z3ASTHandle amount = construct(lse->rightExpr, &shiftWidth);
            return bvVarRightShift(left, amount);
        }
    }

    case ExprKind::ASHR: {
        std::shared_ptr<AShrExpr> ase = std::dynamic_pointer_cast<AShrExpr>(e);
        Z3ASTHandle left = construct(ase->leftExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized ashr");

        if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(ase->rightExpr)) {
            unsigned shift = (unsigned)CE->getLimitedValue();
            Z3ASTHandle signedBool = bvBoolExtract(left, *width_out - 1);
            return constructAShrByConstant(left, shift, signedBool);
        }
        else {
            int shiftWidth;
            Z3ASTHandle amount = construct(ase->rightExpr, &shiftWidth);
            return bvVarArithRightShift(left, amount);
        }
    }

                   // Comparison

    case ExprKind::EQ: {
        std::shared_ptr<EqExpr> ee = std::dynamic_pointer_cast<EqExpr>(e);
        Z3ASTHandle left = construct(ee->leftExpr, width_out);
        Z3ASTHandle right = construct(ee->rightExpr, width_out);
        if (*width_out == 1) {
            if (std::shared_ptr<ConstantExpr> CE = std::dynamic_pointer_cast<ConstantExpr>(ee->leftExpr)) {
                if (CE->isTrue())
                    return right;
                return notExpr(right);
            }
            else {
                return iffExpr(left, right);
            }
        }
        else {
            *width_out = 1;
            return eqExpr(left, right);
        }
    }

    case ExprKind::ULT: {
        std::shared_ptr<UltExpr>ue = std::dynamic_pointer_cast<UltExpr>(e);
        Z3ASTHandle left = construct(ue->leftExpr, width_out);
        Z3ASTHandle right = construct(ue->rightExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized ult");
        *width_out = 1;
        return bvLtExpr(left, right);
    }

    case ExprKind::ULE: {
        std::shared_ptr<UleExpr> ue = std::dynamic_pointer_cast<UleExpr>(e);
        Z3ASTHandle left = construct(ue->leftExpr, width_out);
        Z3ASTHandle right = construct(ue->rightExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized ule");
        *width_out = 1;
        return bvLeExpr(left, right);
    }

    case ExprKind::SLT: {
        std::shared_ptr<SltExpr> se = std::dynamic_pointer_cast<SltExpr>(e);
        Z3ASTHandle left = construct(se->leftExpr, width_out);
        Z3ASTHandle right = construct(se->rightExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized slt");
        *width_out = 1;
        return sbvLtExpr(left, right);
    }

    case ExprKind::SLE: {
        std::shared_ptr<SleExpr> se = std::dynamic_pointer_cast<SleExpr>(e);
        Z3ASTHandle left = construct(se->leftExpr, width_out);
        Z3ASTHandle right = construct(se->rightExpr, width_out);
        assert(*width_out != 1 && "uncanonicalized sle");
        *width_out = 1;
        return sbvLeExpr(left, right);
    }

                  // unused due to canonicalization
#if 0
    case ExprKind::Ne:
    case ExprKind::Ugt:
    case ExprKind::Uge:
    case ExprKind::Sgt:
    case ExprKind::Sge:
#endif

    default:
        assert(0 && "unhandled Expr type");
        return getTrue();
    }
}

void Z3ArrayExprHash::clear()
{
    m_array_hash.clear();
    m_update_node_hash.clear();
}
