#ifndef  EXPR_H
#define EXPR_H

#include <llvm/ADT/APInt.h>
#include <llvm/ADT/APFloat.h>
#include <memory>
#include <llvm/Support/raw_ostream.h>

//   -1 =  minus expr + constant expr


template<typename DerivedType, typename BaseType>
class TIsDerived
{
public:
    static int AnyFunction(BaseType* base)
    {
        return 1;
    }

    static  char AnyFunction(void* t2)
    {
        return 0;
    }

    enum
    {
        Result = (sizeof(int) == sizeof(AnyFunction((DerivedType*)NULL))),
    };
};


enum ExprKind {
    INVALID = -1,
    CONSTANT = 0,

    NOTOPTIMIZED,

    READ,
    SELECT,
    CONCAT,
    EXTRACT,

    ZEXT,
    SEXT,


    // ARITHMETIC
    ADD,
    SUB,
    MUL,
    UDIV,
    SDIV,
    UREM,
    SREM,

    // BIT
    AND,
    NOT,
    OR,
    XOR,
    SHL,
    LSHR,
    ASHR,


    // COMPARE
    EQ,
    NE,  ///< NOT USED IN CANONICAL FORM
    ULT,
    ULE,
    UGT, ///< NOT USED IN CANONICAL FORM
    UGE, ///< NOT USED IN CANONICAL FORM
    SLT,
    SLE,
    SGT, ///< NOT USED IN CANONICAL FORM
    SGE, ///< NOT USED IN CANONICAL FORM

    LAST_KIND = SGE,

    CAST_KIND_FIRST = ZEXT,
    CAST_KIND_LAST = SEXT,
    BINARY_KIND_FIRST = ADD,
    BINARY_KIND_LAST = SGE,
    CMP_KIND_FIRST = EQ,
    CMP_KIND_LAST = SGE
};


class Expr {
public:
    Expr(unsigned int bitNumber,uint64_t value);
    Expr(const llvm::APInt& value);
    virtual ~Expr();

    virtual ExprKind getKind();

    static uint64_t total() { return s_total; }
    uint64_t getWidth() const;

    uint64_t hash() const { return m_hashValue; }
    static std::shared_ptr<Expr>createIsZero(std::shared_ptr<Expr> expr);
    //virtual uint64_t computeHash() = 0;
    static const llvm::fltSemantics* fpWidthToSemantics(uint32_t width);
    static uint32_t getMinBytesForWidth(uint32_t width) { return (width + 7) / 8; }

    bool operator==(const Expr& other) const;
    friend llvm::raw_ostream& operator<<(llvm::raw_ostream& os, const Expr& expr);
public:
    std::shared_ptr<Expr> leftExpr;
    std::shared_ptr<Expr> rightExpr;
protected:
    llvm::APInt m_value;
private:
    static uint64_t s_total;
    uint64_t m_hashValue = 0;
};


template<typename T>
llvm::raw_ostream& operator<<(llvm::raw_ostream& os, const std::shared_ptr<T>& expr) {
    os << *expr;
    return os;
}


bool isPowerOfTwo(uint64_t x);

#endif // ! EXPR_H
